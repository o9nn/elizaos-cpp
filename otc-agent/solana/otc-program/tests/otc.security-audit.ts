/**
 * Security Audit Test Suite for OTC Program
 * 
 * These tests verify that security vulnerabilities have been properly fixed
 * and cover all critical paths through the program.
 */
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
  getAssociatedTokenAddressSync,
} from "@solana/spl-token";
import { assert, expect } from "chai";

describe("OTC Security Audit Tests", () => {
  const provider = anchor.AnchorProvider.env();
  anchor.setProvider(provider);
  const program = anchor.workspace.Otc as Program<Otc>;

  // Test keypairs
  let attacker: Keypair;
  let victim: Keypair;
  let victimDesk: Keypair;
  let attackerDesk: Keypair;
  let tokenMint: PublicKey;
  let usdcMint: PublicKey;
  let victimRegistry: PublicKey;
  let attackerRegistry: PublicKey;
  let victimDeskTokenTreasury: PublicKey;
  let victimDeskUsdcTreasury: PublicKey;

  const airdrop = async (pk: PublicKey, lamports: number) => {
    const sig = await provider.connection.requestAirdrop(pk, lamports);
    await provider.connection.confirmTransaction(sig, "confirmed");
  };

  before(async () => {
    attacker = Keypair.generate();
    victim = Keypair.generate();
    victimDesk = Keypair.generate();
    attackerDesk = Keypair.generate();

    await Promise.all([
      airdrop(attacker.publicKey, 10 * LAMPORTS_PER_SOL),
      airdrop(victim.publicKey, 10 * LAMPORTS_PER_SOL),
    ]);
    await new Promise((r) => setTimeout(r, 1000));

    // Create mints
    tokenMint = await createMint(
      provider.connection,
      attacker,
      attacker.publicKey,
      null,
      9
    );
    usdcMint = await createMint(
      provider.connection,
      attacker,
      attacker.publicKey,
      null,
      6
    );

    // Setup victim desk with inventory
    victimDeskTokenTreasury = getAssociatedTokenAddressSync(
      tokenMint,
      victimDesk.publicKey,
      true
    );
    victimDeskUsdcTreasury = getAssociatedTokenAddressSync(
      usdcMint,
      victimDesk.publicKey,
      true
    );

    await getOrCreateAssociatedTokenAccount(
      provider.connection,
      victim,
      tokenMint,
      victimDesk.publicKey,
      true
    );
    await getOrCreateAssociatedTokenAccount(
      provider.connection,
      victim,
      usdcMint,
      victimDesk.publicKey,
      true
    );

    await program.methods
      .initDesk(new anchor.BN(5 * 1e8), new anchor.BN(1800))
      .accounts({
        payer: victim.publicKey,
        owner: victim.publicKey,
        agent: victim.publicKey,
        usdcMint,
        desk: victimDesk.publicKey,
      })
      .signers([victim, victimDesk])
      .rpc();

    // Deposit tokens to victim desk
    const victimTokenAta = await getOrCreateAssociatedTokenAccount(
      provider.connection,
      victim,
      tokenMint,
      victim.publicKey
    );
    await mintTo(
      provider.connection,
      attacker,
      tokenMint,
      victimTokenAta.address,
      attacker,
      1000000n * 10n ** 9n
    );

    // Register token for victim desk
    [victimRegistry] = PublicKey.findProgramAddressSync(
      [
        Buffer.from("registry"),
        victimDesk.publicKey.toBuffer(),
        tokenMint.toBuffer(),
      ],
      program.programId
    );

    await program.methods
      .registerToken(Array(32).fill(0), PublicKey.default, 0)
      .accounts({
        desk: victimDesk.publicKey,
        payer: victim.publicKey,
        tokenMint,
      })
      .signers([victim])
      .rpc();

    // Set prices
    await program.methods
      .setManualTokenPrice(new anchor.BN(10 * 1e8))
      .accounts({
        tokenRegistry: victimRegistry,
        desk: victimDesk.publicKey,
        owner: victim.publicKey,
      })
      .signers([victim])
      .rpc();

    await program.methods
      .setPrices(
        new anchor.BN(10 * 1e8),
        new anchor.BN(150 * 1e8),
        new anchor.BN(0),
        new anchor.BN(3600)
      )
      .accounts({ desk: victimDesk.publicKey })
      .signers([victim])
      .rpc();

    // Deposit inventory
    await program.methods
      .depositTokens(new anchor.BN(500000n * 10n ** 9n))
      .accounts({
        desk: victimDesk.publicKey,
        tokenRegistry: victimRegistry,
        owner: victim.publicKey,
        ownerTokenAta: victimTokenAta.address,
        deskTokenTreasury: victimDeskTokenTreasury,
      })
      .signers([victim])
      .rpc();

    // Add victim as approver
    await program.methods
      .setApprover(victim.publicKey, true)
      .accounts({ desk: victimDesk.publicKey })
      .signers([victim])
      .rpc();

    // Setup attacker desk
    const attackerDeskTokenTreasury = getAssociatedTokenAddressSync(
      tokenMint,
      attackerDesk.publicKey,
      true
    );
    const attackerDeskUsdcTreasury = getAssociatedTokenAddressSync(
      usdcMint,
      attackerDesk.publicKey,
      true
    );

    await getOrCreateAssociatedTokenAccount(
      provider.connection,
      attacker,
      tokenMint,
      attackerDesk.publicKey,
      true
    );
    await getOrCreateAssociatedTokenAccount(
      provider.connection,
      attacker,
      usdcMint,
      attackerDesk.publicKey,
      true
    );

    await program.methods
      .initDesk(new anchor.BN(1 * 1e8), new anchor.BN(86400))
      .accounts({
        payer: attacker.publicKey,
        owner: attacker.publicKey,
        agent: attacker.publicKey,
        usdcMint,
        desk: attackerDesk.publicKey,
      })
      .signers([attacker, attackerDesk])
      .rpc();

    // Attacker adds themselves as approver
    await program.methods
      .setApprover(attacker.publicKey, true)
      .accounts({ desk: attackerDesk.publicKey })
      .signers([attacker])
      .rpc();

    // Register token on attacker desk with different price
    [attackerRegistry] = PublicKey.findProgramAddressSync(
      [
        Buffer.from("registry"),
        attackerDesk.publicKey.toBuffer(),
        tokenMint.toBuffer(),
      ],
      program.programId
    );

    await program.methods
      .registerToken(Array(32).fill(0), PublicKey.default, 0)
      .accounts({
        desk: attackerDesk.publicKey,
        payer: attacker.publicKey,
        tokenMint,
      })
      .signers([attacker])
      .rpc();

    // Set MUCH LOWER price on attacker registry
    await program.methods
      .setManualTokenPrice(new anchor.BN(1 * 1e8)) // $1 instead of $10
      .accounts({
        tokenRegistry: attackerRegistry,
        desk: attackerDesk.publicKey,
        owner: attacker.publicKey,
      })
      .signers([attacker])
      .rpc();
  });

  describe("CRITICAL: Offer-to-Desk Validation (FIX-1)", () => {
    it("should REJECT approving offer from different desk", async () => {
      // Create legitimate offer on victim desk
      const offer = Keypair.generate();
      const buyer = Keypair.generate();
      await airdrop(buyer.publicKey, 2 * LAMPORTS_PER_SOL);

      // Create offer on victim desk
      await program.methods
        .createOffer(new anchor.BN(100 * 1e9), 0, 1, new anchor.BN(0))
        .accounts({
          desk: victimDesk.publicKey,
          tokenRegistry: victimRegistry,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiary: buyer.publicKey,
          offer: offer.publicKey,
        })
        .signers([buyer, offer])
        .rpc();

      // ATTACK: Try to approve using attacker's desk (where attacker is approver)
      try {
        await program.methods
          .approveOffer(new anchor.BN(1))
          .accounts({
            desk: attackerDesk.publicKey, // Wrong desk
            offer: offer.publicKey,
            approver: attacker.publicKey,
          })
          .signers([attacker])
          .rpc();

        assert.fail("Should have rejected offer from different desk");
      } catch (error: any) {
        assert.include(error.toString(), "BadState");
      }
    });

    it("should REJECT fulfilling offer from different desk", async () => {
      const offer = Keypair.generate();
      const buyer = Keypair.generate();
      await airdrop(buyer.publicKey, 2 * LAMPORTS_PER_SOL);

      // Create and approve offer on victim desk
      await program.methods
        .createOffer(new anchor.BN(100 * 1e9), 0, 1, new anchor.BN(0))
        .accounts({
          desk: victimDesk.publicKey,
          tokenRegistry: victimRegistry,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiary: buyer.publicKey,
          offer: offer.publicKey,
        })
        .signers([buyer, offer])
        .rpc();

      await program.methods
        .approveOffer(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          offer: offer.publicKey,
          approver: victim.publicKey,
        })
        .signers([victim])
        .rpc();

      // Get attacker USDC
      const attackerUsdcAta = await getOrCreateAssociatedTokenAccount(
        provider.connection,
        attacker,
        usdcMint,
        attacker.publicKey
      );
      await mintTo(
        provider.connection,
        attacker,
        usdcMint,
        attackerUsdcAta.address,
        attacker,
        10000n * 10n ** 6n
      );

      const attackerDeskUsdcTreasury = getAssociatedTokenAddressSync(
        usdcMint,
        attackerDesk.publicKey,
        true
      );
      const attackerDeskTokenTreasury = getAssociatedTokenAddressSync(
        tokenMint,
        attackerDesk.publicKey,
        true
      );

      // ATTACK: Try to fulfill using wrong desk
      try {
        await program.methods
          .fulfillOfferUsdc(new anchor.BN(1))
          .accounts({
            desk: attackerDesk.publicKey, // Wrong desk
            offer: offer.publicKey,
            deskTokenTreasury: attackerDeskTokenTreasury,
            deskUsdcTreasury: attackerDeskUsdcTreasury,
            payerUsdcAta: attackerUsdcAta.address,
            payer: attacker.publicKey,
          })
          .signers([attacker])
          .rpc();

        assert.fail("Should have rejected fulfill with wrong desk");
      } catch (error: any) {
        assert.include(error.toString(), "BadState");
      }
    });

    it("should ALLOW approving offer from correct desk", async () => {
      const offer = Keypair.generate();
      const buyer = Keypair.generate();
      await airdrop(buyer.publicKey, 2 * LAMPORTS_PER_SOL);

      await program.methods
        .createOffer(new anchor.BN(100 * 1e9), 0, 1, new anchor.BN(0))
        .accounts({
          desk: victimDesk.publicKey,
          tokenRegistry: victimRegistry,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiary: buyer.publicKey,
          offer: offer.publicKey,
        })
        .signers([buyer, offer])
        .rpc();

      // Approve using correct desk
      await program.methods
        .approveOffer(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          offer: offer.publicKey,
          approver: victim.publicKey,
        })
        .signers([victim])
        .rpc();

      const offerAccount = await program.account.offer.fetch(offer.publicKey);
      assert.isTrue(offerAccount.approved);
    });
  });

  describe("CRITICAL: TokenRegistry-to-Desk Validation (FIX-2)", () => {
    it("should REJECT using TokenRegistry from different desk in createOffer", async () => {
      const buyer = Keypair.generate();
      const offer = Keypair.generate();
      await airdrop(buyer.publicKey, 2 * LAMPORTS_PER_SOL);

      // ATTACK: Try to create offer on victim desk but using attacker's cheap price registry
      try {
        await program.methods
          .createOffer(new anchor.BN(100 * 1e9), 0, 1, new anchor.BN(0))
          .accounts({
            desk: victimDesk.publicKey,
            tokenRegistry: attackerRegistry, // WRONG REGISTRY from different desk
            deskTokenTreasury: victimDeskTokenTreasury,
            beneficiary: buyer.publicKey,
            offer: offer.publicKey,
          })
          .signers([buyer, offer])
          .rpc();

        assert.fail("Should have rejected TokenRegistry from different desk");
      } catch (error: any) {
        assert.include(error.toString(), "BadState");
      }
    });

    it("should REJECT depositing with TokenRegistry from different desk", async () => {
      const depositorTokenAta = await getOrCreateAssociatedTokenAccount(
        provider.connection,
        victim,
        tokenMint,
        victim.publicKey
      );

      try {
        await program.methods
          .depositTokens(new anchor.BN(1000 * 1e9))
          .accounts({
            desk: victimDesk.publicKey,
            tokenRegistry: attackerRegistry, // Wrong registry
            owner: victim.publicKey,
            ownerTokenAta: depositorTokenAta.address,
            deskTokenTreasury: victimDeskTokenTreasury,
          })
          .signers([victim])
          .rpc();

        assert.fail("Should have rejected deposit with wrong registry");
      } catch (error: any) {
        assert.include(error.toString(), "BadState");
      }
    });

    it("should ALLOW using TokenRegistry from correct desk", async () => {
      const buyer = Keypair.generate();
      const offer = Keypair.generate();
      await airdrop(buyer.publicKey, 2 * LAMPORTS_PER_SOL);

      await program.methods
        .createOffer(new anchor.BN(100 * 1e9), 0, 1, new anchor.BN(0))
        .accounts({
          desk: victimDesk.publicKey,
          tokenRegistry: victimRegistry, // Correct registry
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiary: buyer.publicKey,
          offer: offer.publicKey,
        })
        .signers([buyer, offer])
        .rpc();

      const offerAccount = await program.account.offer.fetch(offer.publicKey);
      assert.equal(offerAccount.desk.toBase58(), victimDesk.publicKey.toBase58());
    });
  });

  describe("HIGH: Minimum Quote Expiry (FIX-3)", () => {
    it("should REJECT quote expiry less than 60 seconds", async () => {
      try {
        await program.methods
          .setLimits(
            new anchor.BN(5 * 1e8),
            new anchor.BN(10000 * 1e9),
            new anchor.BN(30), // Only 30 seconds - should fail
            new anchor.BN(0),
            new anchor.BN(365 * 86400)
          )
          .accounts({ desk: victimDesk.publicKey })
          .signers([victim])
          .rpc();

        assert.fail("Should have rejected quote expiry < 60 seconds");
      } catch (error: any) {
        assert.include(error.toString(), "AmountRange");
      }
    });

    it("should ALLOW quote expiry >= 60 seconds", async () => {
      await program.methods
        .setLimits(
          new anchor.BN(5 * 1e8),
          new anchor.BN(10000 * 1e9),
          new anchor.BN(60), // Exactly 60 seconds - should pass
          new anchor.BN(0),
          new anchor.BN(365 * 86400)
        )
        .accounts({ desk: victimDesk.publicKey })
        .signers([victim])
        .rpc();

      const desk = await program.account.desk.fetch(victimDesk.publicKey);
      assert.equal(desk.quoteExpirySecs.toNumber(), 60);
    });
  });

  describe("HIGH: Treasury and ATA Owner Validation (FIX-4)", () => {
    it("should properly validate beneficiary ATA owner in claim", async () => {
      // Create and complete an offer flow to test claim
      const offer = Keypair.generate();
      const buyer = Keypair.generate();
      await airdrop(buyer.publicKey, 5 * LAMPORTS_PER_SOL);

      // Reset limits for longer expiry
      await program.methods
        .setLimits(
          new anchor.BN(5 * 1e8),
          new anchor.BN(10000 * 1e9),
          new anchor.BN(1800),
          new anchor.BN(0),
          new anchor.BN(365 * 86400)
        )
        .accounts({ desk: victimDesk.publicKey })
        .signers([victim])
        .rpc();

      await program.methods
        .createOffer(new anchor.BN(100 * 1e9), 0, 1, new anchor.BN(0))
        .accounts({
          desk: victimDesk.publicKey,
          tokenRegistry: victimRegistry,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiary: buyer.publicKey,
          offer: offer.publicKey,
        })
        .signers([buyer, offer])
        .rpc();

      await program.methods
        .approveOffer(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          offer: offer.publicKey,
          approver: victim.publicKey,
        })
        .signers([victim])
        .rpc();

      // Fund buyer with USDC
      const buyerUsdcAta = await getOrCreateAssociatedTokenAccount(
        provider.connection,
        buyer,
        usdcMint,
        buyer.publicKey
      );
      await mintTo(
        provider.connection,
        attacker,
        usdcMint,
        buyerUsdcAta.address,
        attacker,
        10000n * 10n ** 6n
      );

      // Fulfill offer
      await program.methods
        .fulfillOfferUsdc(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          offer: offer.publicKey,
          deskTokenTreasury: victimDeskTokenTreasury,
          deskUsdcTreasury: victimDeskUsdcTreasury,
          payerUsdcAta: buyerUsdcAta.address,
          payer: buyer.publicKey,
        })
        .signers([buyer])
        .rpc();

      // Create buyer token ATA
      const buyerTokenAta = await getOrCreateAssociatedTokenAccount(
        provider.connection,
        buyer,
        tokenMint,
        buyer.publicKey
      );

      // Create attacker token ATA to try stealing tokens
      const attackerTokenAta = await getOrCreateAssociatedTokenAccount(
        provider.connection,
        attacker,
        tokenMint,
        attacker.publicKey
      );

      // ATTACK: Try to claim tokens to attacker's ATA
      try {
        await program.methods
          .claim(new anchor.BN(1))
          .accounts({
            desk: victimDesk.publicKey,
            deskSigner: victimDesk.publicKey,
            offer: offer.publicKey,
            deskTokenTreasury: victimDeskTokenTreasury,
            beneficiaryTokenAta: attackerTokenAta.address, // WRONG ATA
            beneficiary: buyer.publicKey,
          })
          .signers([victimDesk])
          .rpc();

        assert.fail("Should have rejected claim to wrong beneficiary ATA");
      } catch (error: any) {
        assert.include(error.toString(), "BadState");
      }

      // Claim correctly - should work
      await program.methods
        .claim(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          deskSigner: victimDesk.publicKey,
          offer: offer.publicKey,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiaryTokenAta: buyerTokenAta.address,
          beneficiary: buyer.publicKey,
        })
        .signers([victimDesk])
        .rpc();

      const offerAccount = await program.account.offer.fetch(offer.publicKey);
      assert.isTrue(offerAccount.fulfilled);
    });
  });

  describe("HIGH: SetManualTokenPrice Registry Validation (FIX-5)", () => {
    it("should REJECT setting price on registry from different desk", async () => {
      try {
        // Attacker tries to set price on victim's registry
        await program.methods
          .setManualTokenPrice(new anchor.BN(1 * 1e8))
          .accounts({
            tokenRegistry: victimRegistry, // Victim's registry
            desk: attackerDesk.publicKey, // Attacker's desk
            owner: attacker.publicKey,
          })
          .signers([attacker])
          .rpc();

        assert.fail("Should have rejected setting price on wrong desk's registry");
      } catch (error: any) {
        assert.include(error.toString(), "BadState");
      }
    });
  });

  describe("Access Control Tests", () => {
    it("should REJECT non-owner setting prices", async () => {
      try {
        await program.methods
          .setPrices(
            new anchor.BN(1 * 1e8),
            new anchor.BN(100 * 1e8),
            new anchor.BN(0),
            new anchor.BN(3600)
          )
          .accounts({ desk: victimDesk.publicKey })
          .signers([attacker])
          .rpc();

        assert.fail("Should have rejected non-owner");
      } catch (error: any) {
        assert.include(error.toString().toLowerCase(), "constraint");
      }
    });

    it("should REJECT non-owner setting limits", async () => {
      try {
        await program.methods
          .setLimits(
            new anchor.BN(1 * 1e8),
            new anchor.BN(10000 * 1e9),
            new anchor.BN(60),
            new anchor.BN(0),
            new anchor.BN(365 * 86400)
          )
          .accounts({ desk: victimDesk.publicKey })
          .signers([attacker])
          .rpc();

        assert.fail("Should have rejected non-owner");
      } catch (error: any) {
        assert.include(error.toString().toLowerCase(), "constraint");
      }
    });

    it("should REJECT non-approver approving offer", async () => {
      const offer = Keypair.generate();
      const buyer = Keypair.generate();
      const nonApprover = Keypair.generate();
      await airdrop(buyer.publicKey, 2 * LAMPORTS_PER_SOL);
      await airdrop(nonApprover.publicKey, 2 * LAMPORTS_PER_SOL);

      await program.methods
        .createOffer(new anchor.BN(100 * 1e9), 0, 1, new anchor.BN(0))
        .accounts({
          desk: victimDesk.publicKey,
          tokenRegistry: victimRegistry,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiary: buyer.publicKey,
          offer: offer.publicKey,
        })
        .signers([buyer, offer])
        .rpc();

      try {
        await program.methods
          .approveOffer(new anchor.BN(1))
          .accounts({
            desk: victimDesk.publicKey,
            offer: offer.publicKey,
            approver: nonApprover.publicKey,
          })
          .signers([nonApprover])
          .rpc();

        assert.fail("Should have rejected non-approver");
      } catch (error: any) {
        assert.include(error.toString(), "NotApprover");
      }
    });
  });

  describe("Pause Functionality", () => {
    it("should pause and unpause desk", async () => {
      // Pause
      await program.methods
        .pause()
        .accounts({ desk: victimDesk.publicKey })
        .signers([victim])
        .rpc();

      let desk = await program.account.desk.fetch(victimDesk.publicKey);
      assert.isTrue(desk.paused);

      // Unpause
      await program.methods
        .unpause()
        .accounts({ desk: victimDesk.publicKey })
        .signers([victim])
        .rpc();

      desk = await program.account.desk.fetch(victimDesk.publicKey);
      assert.isFalse(desk.paused);
    });

    it("should REJECT creating offers when paused", async () => {
      await program.methods
        .pause()
        .accounts({ desk: victimDesk.publicKey })
        .signers([victim])
        .rpc();

      const offer = Keypair.generate();
      const buyer = Keypair.generate();
      await airdrop(buyer.publicKey, 2 * LAMPORTS_PER_SOL);

      try {
        await program.methods
          .createOffer(new anchor.BN(100 * 1e9), 0, 1, new anchor.BN(0))
          .accounts({
            desk: victimDesk.publicKey,
            tokenRegistry: victimRegistry,
            deskTokenTreasury: victimDeskTokenTreasury,
            beneficiary: buyer.publicKey,
            offer: offer.publicKey,
          })
          .signers([buyer, offer])
          .rpc();

        assert.fail("Should have rejected when paused");
      } catch (error: any) {
        assert.include(error.toString(), "Paused");
      }

      // Unpause for other tests
      await program.methods
        .unpause()
        .accounts({ desk: victimDesk.publicKey })
        .signers([victim])
        .rpc();
    });
  });

  describe("Complete Secure OTC Flow", () => {
    it("should complete a secure OTC transaction end-to-end", async () => {
      const buyer = Keypair.generate();
      const offer = Keypair.generate();
      await airdrop(buyer.publicKey, 5 * LAMPORTS_PER_SOL);

      // 1. Create offer with discount
      await program.methods
        .createOffer(new anchor.BN(50 * 1e9), 500, 1, new anchor.BN(0)) // 5% discount
        .accounts({
          desk: victimDesk.publicKey,
          tokenRegistry: victimRegistry,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiary: buyer.publicKey,
          offer: offer.publicKey,
        })
        .signers([buyer, offer])
        .rpc();

      // Verify offer state
      let offerAccount = await program.account.offer.fetch(offer.publicKey);
      assert.equal(offerAccount.desk.toBase58(), victimDesk.publicKey.toBase58());
      assert.equal(offerAccount.tokenAmount.toNumber(), 50 * 1e9);
      assert.equal(offerAccount.discountBps, 500);
      assert.isFalse(offerAccount.approved);

      // 2. Approve offer
      await program.methods
        .approveOffer(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          offer: offer.publicKey,
          approver: victim.publicKey,
        })
        .signers([victim])
        .rpc();

      offerAccount = await program.account.offer.fetch(offer.publicKey);
      assert.isTrue(offerAccount.approved);

      // 3. Fund buyer and fulfill
      const buyerUsdcAta = await getOrCreateAssociatedTokenAccount(
        provider.connection,
        buyer,
        usdcMint,
        buyer.publicKey
      );
      await mintTo(
        provider.connection,
        attacker,
        usdcMint,
        buyerUsdcAta.address,
        attacker,
        10000n * 10n ** 6n
      );

      await program.methods
        .fulfillOfferUsdc(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          offer: offer.publicKey,
          deskTokenTreasury: victimDeskTokenTreasury,
          deskUsdcTreasury: victimDeskUsdcTreasury,
          payerUsdcAta: buyerUsdcAta.address,
          payer: buyer.publicKey,
        })
        .signers([buyer])
        .rpc();

      offerAccount = await program.account.offer.fetch(offer.publicKey);
      assert.isTrue(offerAccount.paid);
      assert.isTrue(offerAccount.amountPaid.toNumber() > 0);

      // 4. Claim tokens
      const buyerTokenAta = await getOrCreateAssociatedTokenAccount(
        provider.connection,
        buyer,
        tokenMint,
        buyer.publicKey
      );

      await program.methods
        .claim(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          deskSigner: victimDesk.publicKey,
          offer: offer.publicKey,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiaryTokenAta: buyerTokenAta.address,
          beneficiary: buyer.publicKey,
        })
        .signers([victimDesk])
        .rpc();

      // Verify final state
      offerAccount = await program.account.offer.fetch(offer.publicKey);
      assert.isTrue(offerAccount.paid);
      assert.isTrue(offerAccount.fulfilled);
      assert.isFalse(offerAccount.cancelled);

      // Verify buyer received tokens
      const buyerTokenBalance = await provider.connection.getTokenAccountBalance(
        buyerTokenAta.address
      );
      assert.equal(buyerTokenBalance.value.amount, (50n * 10n ** 9n).toString());
    });

    it("should complete secure SOL payment flow", async () => {
      const buyer = Keypair.generate();
      const offer = Keypair.generate();
      await airdrop(buyer.publicKey, 10 * LAMPORTS_PER_SOL);

      // 1. Create offer for SOL payment
      await program.methods
        .createOffer(new anchor.BN(10 * 1e9), 0, 0, new anchor.BN(0)) // currency=0 for SOL
        .accounts({
          desk: victimDesk.publicKey,
          tokenRegistry: victimRegistry,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiary: buyer.publicKey,
          offer: offer.publicKey,
        })
        .signers([buyer, offer])
        .rpc();

      // 2. Approve
      await program.methods
        .approveOffer(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          offer: offer.publicKey,
          approver: victim.publicKey,
        })
        .signers([victim])
        .rpc();

      // 3. Fulfill with SOL
      await program.methods
        .fulfillOfferSol(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          offer: offer.publicKey,
          deskTokenTreasury: victimDeskTokenTreasury,
          payer: buyer.publicKey,
        })
        .signers([buyer])
        .rpc();

      const offerAccount = await program.account.offer.fetch(offer.publicKey);
      assert.isTrue(offerAccount.paid);
      assert.isTrue(offerAccount.amountPaid.toNumber() > 0);

      // 4. Claim
      const buyerTokenAta = await getOrCreateAssociatedTokenAccount(
        provider.connection,
        buyer,
        tokenMint,
        buyer.publicKey
      );

      await program.methods
        .claim(new anchor.BN(1))
        .accounts({
          desk: victimDesk.publicKey,
          deskSigner: victimDesk.publicKey,
          offer: offer.publicKey,
          deskTokenTreasury: victimDeskTokenTreasury,
          beneficiaryTokenAta: buyerTokenAta.address,
          beneficiary: buyer.publicKey,
        })
        .signers([victimDesk])
        .rpc();

      const finalOffer = await program.account.offer.fetch(offer.publicKey);
      assert.isTrue(finalOffer.fulfilled);
    });
  });
});
