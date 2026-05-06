import type { Program } from "@coral-xyz/anchor";
import pkg from "@coral-xyz/anchor";
import type { Otc } from "../target/types/otc";
import { PublicKey, SystemProgram, Keypair } from "@solana/web3.js";
import { TOKEN_PROGRAM_ID, getAssociatedTokenAddressSync, createMint, getOrCreateAssociatedTokenAccount, mintTo } from "@solana/spl-token";
import { expect } from "chai";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace, BN } = pkg as typeof import("@coral-xyz/anchor");

describe("otc flows", () => {
  const provider = AnchorProvider.env();
  setProvider(provider);
  const program = workspace.Otc as Program<Otc>;

  const airdrop = async (pk: PublicKey, lamports: number) => {
    const sig = await provider.connection.requestAirdrop(pk, lamports);
    await provider.connection.confirmTransaction(sig, "confirmed");
  };

  // Derive token registry PDA
  const getTokenRegistryPda = (desk: PublicKey, tokenMint: PublicKey) => {
    return PublicKey.findProgramAddressSync(
      [Buffer.from("registry"), desk.toBuffer(), tokenMint.toBuffer()],
      program.programId
    )[0];
  };

  let owner: Keypair;
  let agent: Keypair;
  let tokenMint: PublicKey;
  let usdcMint: PublicKey;
  let desk: Keypair;
  let deskTokenTreasury: PublicKey;
  let deskUsdcTreasury: PublicKey;
  let tokenRegistry: PublicKey;

  before(async () => {
    owner = Keypair.generate();
    await airdrop(owner.publicKey, 2e9);
    agent = Keypair.generate();
    await airdrop(agent.publicKey, 2e9);
    desk = Keypair.generate();
    tokenMint = await createMint(provider.connection, owner, owner.publicKey, null, 9);
    usdcMint = await createMint(provider.connection, owner, owner.publicKey, null, 6);

    deskTokenTreasury = getAssociatedTokenAddressSync(tokenMint, desk.publicKey, true);
    deskUsdcTreasury = getAssociatedTokenAddressSync(usdcMint, desk.publicKey, true);
    tokenRegistry = getTokenRegistryPda(desk.publicKey, tokenMint);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, tokenMint, desk.publicKey, true);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, usdcMint, desk.publicKey, true);

    // Initialize desk without tokenMint (multi-token architecture)
    await program.methods
      .initDesk(new BN(500000000), new BN(1800))
      .accounts({ owner: owner.publicKey, agent: agent.publicKey, usdcMint, desk: desk.publicKey, payer: owner.publicKey })
      .signers([owner, desk])
      .rpc();

    // Register the token with desk
    const dummyPriceFeedId = new Array(32).fill(0);
    await program.methods
      .registerToken(dummyPriceFeedId, tokenMint, 0) // poolAddress = tokenMint as placeholder, 0=PoolType::None
      .accounts({ desk: desk.publicKey, payer: owner.publicKey, tokenMint })
      .signers([owner])
      .rpc();

    await program.methods.setPrices(new BN(1_000_000_000), new BN(100_000_000_00), new BN(0), new BN(3600)).accounts({ desk: desk.publicKey }).signers([owner]).rpc();

    // mint and deposit token inventory
    const ownerTokenAta = getAssociatedTokenAddressSync(tokenMint, owner.publicKey);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, tokenMint, owner.publicKey);
    await mintTo(provider.connection, owner, tokenMint, ownerTokenAta, owner, 1_000_000_000000000n);
    await program.methods.depositTokens(new BN("500000000000000")).accounts({ desk: desk.publicKey, tokenRegistry, owner: owner.publicKey, ownerTokenAta, deskTokenTreasury }).signers([owner]).rpc();
  });

  it("USDC: create -> approve -> fulfill -> claim", async () => {
    const beneficiary = Keypair.generate();
    await airdrop(beneficiary.publicKey, 2e9);
    const deskAccount = await program.account.desk.fetch(desk.publicKey);
    const id = new BN(deskAccount.nextOfferId.toString());
    const offer = Keypair.generate();

    await program.methods
      .createOffer(new BN("1000000000"), 0, 1, new BN(0))
      .accounts({ desk: desk.publicKey, tokenRegistry, deskTokenTreasury, beneficiary: beneficiary.publicKey, offer: offer.publicKey })
      .signers([beneficiary, offer])
      .rpc();

    await program.methods.setApprover(beneficiary.publicKey, true).accounts({ desk: desk.publicKey }).signers([owner]).rpc();
    await program.methods.approveOffer(id).accounts({ desk: desk.publicKey, offer: offer.publicKey }).signers([beneficiary]).rpc();

    const payerUsdc = await getOrCreateAssociatedTokenAccount(provider.connection, owner, usdcMint, beneficiary.publicKey);
    await mintTo(provider.connection, owner, usdcMint, payerUsdc.address, owner, 1_000_000_000n);
    await program.methods.fulfillOfferUsdc(id).accounts({ desk: desk.publicKey, offer: offer.publicKey, deskTokenTreasury, deskUsdcTreasury, payerUsdcAta: payerUsdc.address, payer: beneficiary.publicKey }).signers([beneficiary]).rpc();

    const beneficiaryTokenAta = getAssociatedTokenAddressSync(tokenMint, beneficiary.publicKey);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, tokenMint, beneficiary.publicKey);
    await program.methods.claim(id).accounts({ desk: desk.publicKey, deskSigner: desk.publicKey, offer: offer.publicKey, deskTokenTreasury, beneficiaryTokenAta, beneficiary: beneficiary.publicKey }).signers([desk]).rpc();

    const bal = await provider.connection.getTokenAccountBalance(beneficiaryTokenAta);
    expect(parseInt(bal.value.amount)).to.be.greaterThan(0);
  });

  it("SOL: create -> approve -> fulfill -> claim", async () => {
    const user = Keypair.generate();
    await airdrop(user.publicKey, 2e9);
    const deskAccount2 = await program.account.desk.fetch(desk.publicKey);
    const id2 = new BN(deskAccount2.nextOfferId.toString());
    const offer = Keypair.generate();

    await program.methods
      .createOffer(new BN("500000000"), 0, 0, new BN(0))
      .accounts({ desk: desk.publicKey, tokenRegistry, deskTokenTreasury, beneficiary: user.publicKey, offer: offer.publicKey })
      .signers([user, offer])
      .rpc();
    await program.methods.setApprover(user.publicKey, true).accounts({ desk: desk.publicKey }).signers([owner]).rpc();
    await program.methods.approveOffer(id2).accounts({ desk: desk.publicKey, offer: offer.publicKey }).signers([user]).rpc();

    await program.methods.fulfillOfferSol(id2).accounts({ desk: desk.publicKey, offer: offer.publicKey, deskTokenTreasury, payer: user.publicKey }).signers([user]).rpc();

    const userTokenAta = getAssociatedTokenAddressSync(tokenMint, user.publicKey);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, tokenMint, user.publicKey);
    await program.methods.claim(id2).accounts({ desk: desk.publicKey, deskSigner: desk.publicKey, offer: offer.publicKey, deskTokenTreasury, beneficiaryTokenAta: userTokenAta, beneficiary: user.publicKey }).signers([desk]).rpc();

    const bal = await provider.connection.getTokenAccountBalance(userTokenAta);
    expect(parseInt(bal.value.amount)).to.be.greaterThan(0);
  });
});
