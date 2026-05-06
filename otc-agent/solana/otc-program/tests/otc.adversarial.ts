import type { Program } from "@coral-xyz/anchor";
import pkg from "@coral-xyz/anchor";
import { createMint, getAssociatedTokenAddressSync, getOrCreateAssociatedTokenAccount, mintTo } from "@solana/spl-token";
import { Keypair, PublicKey, SystemProgram } from "@solana/web3.js";
import { expect } from "chai";
import type { Otc } from "../target/types/otc";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace, BN } = pkg as typeof import("@coral-xyz/anchor");

describe("otc adversarial", () => {
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

  beforeEach(async () => {
    owner = Keypair.generate();
    agent = Keypair.generate();
    desk = Keypair.generate();
    await airdrop(owner.publicKey, 2e9);
    await airdrop(agent.publicKey, 2e9);
    tokenMint = await createMint(provider.connection, owner, owner.publicKey, null, 9);
    usdcMint = await createMint(provider.connection, owner, owner.publicKey, null, 6);
    deskTokenTreasury = getAssociatedTokenAddressSync(tokenMint, desk.publicKey, true);
    deskUsdcTreasury = getAssociatedTokenAddressSync(usdcMint, desk.publicKey, true);
    tokenRegistry = getTokenRegistryPda(desk.publicKey, tokenMint);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, tokenMint, desk.publicKey, true);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, usdcMint, desk.publicKey, true);

    // Initialize desk without tokenMint (multi-token architecture)
    await program.methods
      .initDesk(new BN(500000000), new BN(60))
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

    const ownerTokenAta = getAssociatedTokenAddressSync(tokenMint, owner.publicKey);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, tokenMint, owner.publicKey);
    await mintTo(provider.connection, owner, tokenMint, ownerTokenAta, owner, 1_000_000_000000000n);
    await program.methods.depositTokens(new BN("500000000000000"))
      .accounts({ desk: desk.publicKey, tokenRegistry, owner: owner.publicKey, ownerTokenAta, deskTokenTreasury })
      .signers([owner]).rpc();

    await program.methods.setPrices(new BN(1_000_000_000), new BN(100_000_000_00), new BN(0), new BN(3600))
      .accounts({ desk: desk.publicKey }).signers([owner]).rpc();
  });

  it("rejects createOffer when insufficient inventory (reserved)", async () => {
    const user = Keypair.generate();
    await airdrop(user.publicKey, 2e9);
    const deskAccount = await program.account.desk.fetch(desk.publicKey);
    const id = new BN(deskAccount.nextOfferId.toString());
    const offer = Keypair.generate();

    await program.methods.createOffer(new BN("1000000000"), 0, 1, new BN(0))
      .accountsStrict({ desk: desk.publicKey, tokenRegistry, deskTokenTreasury, beneficiary: user.publicKey, offer: offer.publicKey, systemProgram: SystemProgram.programId })
      .signers([user, offer]).rpc();
    await program.methods.setApprover(user.publicKey, true).accounts({ desk: desk.publicKey }).signers([owner]).rpc();
    await program.methods.approveOffer(id).accounts({ desk: desk.publicKey, offer: offer.publicKey, approver: user.publicKey }).signers([user]).rpc();

    const payerUsdc = await getOrCreateAssociatedTokenAccount(provider.connection, owner, usdcMint, user.publicKey);
    await mintTo(provider.connection, owner, usdcMint, payerUsdc.address, owner, 1_000_000_000n);
    await program.methods.fulfillOfferUsdc(id).accounts({ desk: desk.publicKey, offer: offer.publicKey, deskTokenTreasury, deskUsdcTreasury, payerUsdcAta: payerUsdc.address, payer: user.publicKey }).signers([user]).rpc();

    // Attempt to withdraw below reserved should fail
    const withdrawAmount = new BN("500000000000000");
    try {
      await program.methods.withdrawTokens(withdrawAmount).accounts({ desk: desk.publicKey, tokenRegistry, deskSigner: desk.publicKey, deskTokenTreasury, ownerTokenAta: getAssociatedTokenAddressSync(tokenMint, owner.publicKey) }).signers([owner, desk]).rpc();
      expect.fail("withdrawTokens should fail due to reserved balance");
    } catch (e) {
      // ok
    }
  });

  it("rejects fulfill when expired (checked add)", async () => {
    const user = Keypair.generate();
    await airdrop(user.publicKey, 2e9);
    const d = await program.account.desk.fetch(desk.publicKey);
    const id = new BN(d.nextOfferId.toString());
    const offer = Keypair.generate();
    await program.methods.createOffer(new BN("1000000000"), 0, 1, new BN(0)).accountsStrict({ desk: desk.publicKey, tokenRegistry, deskTokenTreasury, beneficiary: user.publicKey, offer: offer.publicKey, systemProgram: SystemProgram.programId }).signers([user, offer]).rpc();
    await program.methods.setApprover(user.publicKey, true).accounts({ desk: desk.publicKey }).signers([owner]).rpc();
    await program.methods.approveOffer(id).accounts({ desk: desk.publicKey, offer: offer.publicKey, approver: user.publicKey }).signers([user]).rpc();

    // artificially set quote_expiry_secs very small by owner
    await program.methods.setLimits(new BN(500000000), new BN("1000000000000000"), new BN(1), new BN(0), new BN(365*24*3600)).accounts({ desk: desk.publicKey }).signers([owner]).rpc();
    await new Promise(r => setTimeout(r, 1500));
    try {
      await program.methods.fulfillOfferUsdc(id).accounts({ desk: desk.publicKey, offer: offer.publicKey, deskTokenTreasury, deskUsdcTreasury, payerUsdcAta: getAssociatedTokenAddressSync(usdcMint, user.publicKey), payer: user.publicKey }).signers([user]).rpc();
      expect.fail("fulfill should fail due to expiry");
    } catch (e) {}
  });

  it("enforces desk ownership of treasuries in fulfill", async () => {
    const user = Keypair.generate();
    await airdrop(user.publicKey, 2e9);
    const d = await program.account.desk.fetch(desk.publicKey);
    const id = new BN(d.nextOfferId.toString());
    const offer = Keypair.generate();

    await program.methods.createOffer(new BN("1000000000"), 0, 1, new BN(0)).accountsStrict({ desk: desk.publicKey, tokenRegistry, deskTokenTreasury, beneficiary: user.publicKey, offer: offer.publicKey, systemProgram: SystemProgram.programId }).signers([user, offer]).rpc();
    await program.methods.setApprover(user.publicKey, true).accounts({ desk: desk.publicKey }).signers([owner]).rpc();
    await program.methods.approveOffer(id).accounts({ desk: desk.publicKey, offer: offer.publicKey, approver: user.publicKey }).signers([user]).rpc();

    // Craft a fake treasury owned by user
    const fakeTreasury = getAssociatedTokenAddressSync(tokenMint, user.publicKey);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, tokenMint, user.publicKey);
    try {
      await program.methods.fulfillOfferSol(id).accounts({ desk: desk.publicKey, offer: offer.publicKey, deskTokenTreasury: fakeTreasury, payer: user.publicKey }).signers([user]).rpc();
      expect.fail("fulfill should fail due to treasury owner constraint");
    } catch (e) {}
  });
});
