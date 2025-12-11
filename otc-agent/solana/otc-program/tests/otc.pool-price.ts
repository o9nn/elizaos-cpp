import * as anchor from "@coral-xyz/anchor";
import { Program } from "@coral-xyz/anchor";
import { Otc } from "../target/types/otc";
import {
  Keypair,
  PublicKey,
  SystemProgram,
  LAMPORTS_PER_SOL,
} from "@solana/web3.js";
import {
  createMint,
  getOrCreateAssociatedTokenAccount,
  mintTo,
  TOKEN_PROGRAM_ID,
  createAccount,
} from "@solana/spl-token";
import { assert } from "chai";

describe("OTC Pool Price Tests", () => {
  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);

  const program = anchor.workspace.Otc as Program<Otc>;

  let desk: Keypair;
  let owner: Keypair;
  let payer: Keypair;
  
  let tokenMint: PublicKey;
  let quoteMint: PublicKey; // USDC
  
  let poolAddress: PublicKey;
  let vaultA: PublicKey;
  let vaultB: PublicKey;
  
  let tokenRegistry: PublicKey;

  before(async () => {
    desk = Keypair.generate();
    owner = Keypair.generate();
    payer = Keypair.generate();
    
    // Airdrop
    await provider.connection.requestAirdrop(owner.publicKey, 10 * LAMPORTS_PER_SOL);
    await provider.connection.requestAirdrop(payer.publicKey, 10 * LAMPORTS_PER_SOL);
    await new Promise(resolve => setTimeout(resolve, 1000));

    // Create mints
    // Token A: 9 decimals (Solana standard)
    tokenMint = await createMint(
      provider.connection,
      owner,
      owner.publicKey,
      null,
      9
    );

    // Token B (Quote/USDC): 6 decimals
    quoteMint = await createMint(
      provider.connection,
      owner,
      owner.publicKey,
      null,
      6
    );

    // Initialize Desk (multi-token architecture - no tokenMint)
    await program.methods
      .initDesk(
        new anchor.BN(5 * 1e8), // min $5
        new anchor.BN(1800)
      )
      .accounts({
        payer: owner.publicKey,
        owner: owner.publicKey,
        agent: owner.publicKey,
        usdcMint: quoteMint,
        desk: desk.publicKey,
      })
      .signers([owner, desk])
      .rpc();
      
    // Setup Mock Pool
    // In reality, pool is a PDA or program account. Here we just need an address that matches registry.
    poolAddress = Keypair.generate().publicKey;
    
    // Create vaults (TokenAccounts)
    // vaultA holds Token A
    // vaultB holds Quote Token (USDC)
    
    // We need these vaults to be owned by a program? 
    // The contract checks: require!(vault_a.owner == ctx.accounts.token_program.key(), OtcError::BadState);
    // Wait, TokenAccount.owner usually refers to the user wallet (authority).
    // But in `Account<'info, TokenAccount>`, the `owner` field is the Program ID (Token Program).
    // The contract code: `vault_a.owner` access in Anchor might refer to the `owner` field of the struct, which is the `authority`.
    // Let's check the contract code again.
    // `vault_a` is `Account<'info, TokenAccount>`.
    // `vault_a.owner` refers to the `owner` field of the `TokenAccount` struct, which is the wallet/authority.
    // The check `require!(vault_a.owner == ctx.accounts.token_program.key(), OtcError::BadState);` 
    // means the AUTHORITY of the token account must be the Token Program? That makes no sense.
    // Token Program ID is the owner of the Account Info (the executable/program that owns the data).
    // The `owner` field in `TokenAccount` struct is the user who can spend tokens.
    
    // Let's re-read the contract code I wrote.
    /*
        let vault_a = &ctx.accounts.vault_a;
        require!(vault_a.owner == ctx.accounts.token_program.key(), OtcError::BadState);
    */
    // In Anchor `Account<'info, TokenAccount>`, fields match the on-chain data.
    // `vault_a.owner` is the authority.
    // `token_program.key()` is `TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA`.
    // So I am checking if the AUTHORITY of the vault is the Token Program?
    // Usually vaults in AMMs are owned (authority) by the AMM PDA.
    // They are NOT owned by the Token Program itself as an authority.
    // The Token Program "owns" the account in the Solana runtime sense (AccountInfo.owner).
    
    // If `vault_a` is `Account<'info, TokenAccount>`, then `vault_a` dereferences to the `TokenAccount` struct.
    // The struct has an `owner` field which is the authority.
    // My check seems wrong if I intended to check runtime ownership. 
    // But `Account` wrapper already ensures `owner` (runtime) is Token Program.
    // If I wanted to check if the account exists and is a token account, `Account<'info, TokenAccount>` does that.
    
    // If I wanted to check if `pool` owns the vaults (which is typical for AMMs), I should check `vault_a.owner == pool` (or pool signer).
    // But my code checked `vault_a.owner == token_program.key()`. This means the authority is the token program?
    // That is practically impossible/useless.
    
    // I should probably fix the contract logic to check `vault_a.owner == pool` or something similar if I want to enforce relationship.
    // Or just remove the check if I want to be generic and purely permissionless/trusting the registry config.
    
    // For now, to make the test pass with CURRENT contract logic:
    // I need to set the vault authority to be the Token Program ID? 
    // Can I do that? `createAccount` allows specifying owner.
    
    // Wait, if the contract logic is broken/impossible, I should fix the contract first.
    // Let's check the contract logic in `lib.rs` again.
  });

  it("registers token permissionlessly with pool", async () => {
    // Derive PDA for registry
    const [registryPda] = PublicKey.findProgramAddressSync(
      [Buffer.from("registry"), desk.publicKey.toBuffer(), tokenMint.toBuffer()],
      program.programId
    );
    tokenRegistry = registryPda;

    const priceFeedId = Buffer.alloc(32).fill(1); // Dummy feed ID

    await program.methods
      .registerToken([...priceFeedId], poolAddress, 1)
      .accounts({
        desk: desk.publicKey,
        payer: payer.publicKey,
        tokenMint: tokenMint,
        // systemProgram: SystemProgram.programId,
      })
      .signers([payer])
      .rpc();

    const regAccount = await program.account.tokenRegistry.fetch(registryPda);
    assert.equal(regAccount.desk.toBase58(), desk.publicKey.toBase58());
    assert.equal(regAccount.tokenMint.toBase58(), tokenMint.toBase58());
    assert.equal(regAccount.poolAddress.toBase58(), poolAddress.toBase58());
    assert.equal(regAccount.registeredBy.toBase58(), payer.publicKey.toBase58());
  });

  it("updates price from pool", async () => {
    // Create vaults owned by poolAddress
    // vaultA (Token): 100 tokens (100 * 10^9)
    vaultA = await createAccount(
      provider.connection,
      payer,
      tokenMint,
      poolAddress, // owner
      Keypair.generate()
    );
    
    // vaultB (Quote/USDC): 200 USDC (200 * 10^6)
    vaultB = await createAccount(
      provider.connection,
      payer,
      quoteMint,
      poolAddress, // owner
      Keypair.generate()
    );

    // Fund vaults
    await mintTo(provider.connection, payer, tokenMint, vaultA, owner, 100 * 10**9);
    await mintTo(provider.connection, payer, quoteMint, vaultB, owner, 200 * 10**6);

    // Price = 200 / 100 = $2.00
    // Expected 8 decimals: 200,000,000

    await program.methods
      .updateTokenPriceFromPool()
      .accounts({
        tokenRegistry: tokenRegistry,
        pool: poolAddress,
        vaultA: vaultA,
        vaultB: vaultB,
        // tokenProgram: TOKEN_PROGRAM_ID,
      })
      .rpc();

    const regAccount = await program.account.tokenRegistry.fetch(tokenRegistry);
    const price = regAccount.tokenUsdPrice8d.toNumber();
    
    // vaultB (Quote) is numerator? 
    // Logic: price = amount_b * 10^8 * 10^token_dec / (amount_a * 10^quote_dec)
    // amount_b = 200 * 10^6
    // amount_a = 100 * 10^9
    // num = 200 * 10^6 * 10^8 * 10^9 = 200 * 10^23
    // den = 100 * 10^9 * 10^6 = 100 * 10^15
    // result = 2 * 10^8 = 200,000,000 ($2.00)
    
    assert.equal(price, 200_000_000);
    assert.ok(regAccount.pricesUpdatedAt.toNumber() > 0);
  });
});

