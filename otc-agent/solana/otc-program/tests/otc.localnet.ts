import type { Program } from "@coral-xyz/anchor";
import pkg from "@coral-xyz/anchor";
import type { Otc } from "../target/types/otc";
import { PublicKey, SystemProgram, Keypair } from "@solana/web3.js";
import { TOKEN_PROGRAM_ID, getAssociatedTokenAddressSync, createMint, getOrCreateAssociatedTokenAccount, mintTo } from "@solana/spl-token";
import { expect } from "chai";

// ESM/CJS compatibility: import as default then destructure
const { AnchorProvider, setProvider, workspace, BN } = pkg as typeof import("@coral-xyz/anchor");

describe("otc localnet smoke", () => {
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

  it("initializes desk and deposits tokens", async () => {
    const owner = Keypair.generate();
    const sig = await provider.connection.requestAirdrop(owner.publicKey, 2e9);
    await provider.connection.confirmTransaction(sig, "confirmed");
    const agent = Keypair.generate();
    await airdrop(agent.publicKey, 2e9);
    const desk = Keypair.generate();

    const tokenMint = await createMint(provider.connection, owner, owner.publicKey, null, 9);
    const usdcMint = await createMint(provider.connection, owner, owner.publicKey, null, 6);

    const deskTokenTreasury = getAssociatedTokenAddressSync(tokenMint, desk.publicKey, true);
    const deskUsdcTreasury = getAssociatedTokenAddressSync(usdcMint, desk.publicKey, true);
    const tokenRegistry = getTokenRegistryPda(desk.publicKey, tokenMint);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, tokenMint, desk.publicKey, true);
    await getOrCreateAssociatedTokenAccount(provider.connection, owner, usdcMint, desk.publicKey, true);

    // Initialize desk without tokenMint (multi-token architecture)
    await program.methods
      .initDesk(new BN(500000000), new BN(1800))
      .accounts({ desk: desk.publicKey, usdcMint, owner: owner.publicKey, agent: agent.publicKey, payer: owner.publicKey })
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

    await program.methods.depositTokens(new BN("500000000000000")).accounts({ desk: desk.publicKey, tokenRegistry, owner: owner.publicKey, ownerTokenAta, deskTokenTreasury }).signers([owner]).rpc();

    expect(true).to.eq(true);
  });
});
