import { Connection, Keypair, PublicKey, Transaction, TransactionInstruction, SystemProgram } from '@solana/web3.js';
import { TOKEN_2022_PROGRAM_ID, createInitializeMintInstruction, createInitializeAccount2Instruction, getMintLen, getAccountLen, ExtensionType, createInitializeMintCloseAuthorityInstruction, createInitializeTransferFeeConfigInstruction, createInitializePermanentDelegateInstruction, createInitializeMetadataPointerInstruction, createInitializeGroupMemberPointerInstruction } from '@solana/spl-token';
import bs58 from 'bs58';

export async function executeSkill(blockhash: string): Promise<string> {
  const connection = new Connection('http://localhost:8899', 'confirmed');
  const payerSecretKey = new Uint8Array([208,175,150,242,88,34,108,88,177,16,168,75,115,181,199,242,120,4,78,75,19,227,186,136,196,250,195,30,69,226,91,245,251,52,243,87,37,157,226,234,190,241,175,62,60,79,194,81,155,136,220,167,24,221,144,154,88,140,17,134,61,84,50,9]);
  const payer = Keypair.fromSecretKey(payerSecretKey);
  
  const transaction = new Transaction();
  transaction.recentBlockhash = blockhash;
  transaction.feePayer = payer.publicKey;

  // Token-2022: InitializeMint (disc 0)
  const mint1 = Keypair.generate();
  const mintLen1 = getMintLen([]);
  transaction.add(
    SystemProgram.createAccount({
      fromPubkey: payer.publicKey,
      newAccountPubkey: mint1.publicKey,
      lamports: await connection.getMinimumBalanceForRentExemption(mintLen1),
      space: mintLen1,
      programId: TOKEN_2022_PROGRAM_ID,
    })
  );
  transaction.add(
    createInitializeMintInstruction(
      mint1.publicKey,
      9,
      payer.publicKey,
      payer.publicKey,
      TOKEN_2022_PROGRAM_ID
    )
  );

  // Token-2022: Burn (disc 8)
  const burnData = Buffer.alloc(9);
  burnData.writeUInt8(8, 0);
  burnData.writeBigUInt64LE(0n, 1);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: mint1.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: burnData,
    })
  );

  // Token-2022: CloseAccount (disc 9)
  const closeData = Buffer.alloc(1);
  closeData.writeUInt8(9, 0);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: closeData,
    })
  );

  // Token-2022: ApproveChecked (disc 13)
  const approveData = Buffer.alloc(10);
  approveData.writeUInt8(13, 0);
  approveData.writeBigUInt64LE(0n, 1);
  approveData.writeUInt8(9, 9);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: mint1.publicKey, isSigner: false, isWritable: false },
        { pubkey: payer.publicKey, isSigner: false, isWritable: false },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: approveData,
    })
  );

  // Token-2022: MintToChecked (disc 14)
  const mintToData = Buffer.alloc(10);
  mintToData.writeUInt8(14, 0);
  mintToData.writeBigUInt64LE(0n, 1);
  mintToData.writeUInt8(9, 9);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: mint1.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: mintToData,
    })
  );

  // Token-2022: InitializeAccount2 (disc 16)
  const account1 = Keypair.generate();
  const accountLen1 = getAccountLen([]);
  transaction.add(
    SystemProgram.createAccount({
      fromPubkey: payer.publicKey,
      newAccountPubkey: account1.publicKey,
      lamports: await connection.getMinimumBalanceForRentExemption(accountLen1),
      space: accountLen1,
      programId: TOKEN_2022_PROGRAM_ID,
    })
  );
  transaction.add(
    createInitializeAccount2Instruction(
      account1.publicKey,
      mint1.publicKey,
      payer.publicKey,
      TOKEN_2022_PROGRAM_ID
    )
  );

  // Token-2022: GetAccountDataSize (disc 21)
  const getDataSizeData = Buffer.alloc(1);
  getDataSizeData.writeUInt8(21, 0);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: mint1.publicKey, isSigner: false, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: getDataSizeData,
    })
  );

  // Token-2022: InitializeImmutableOwner (disc 22)
  const account2 = Keypair.generate();
  const accountLen2 = getAccountLen([ExtensionType.ImmutableOwner]);
  transaction.add(
    SystemProgram.createAccount({
      fromPubkey: payer.publicKey,
      newAccountPubkey: account2.publicKey,
      lamports: await connection.getMinimumBalanceForRentExemption(accountLen2),
      space: accountLen2,
      programId: TOKEN_2022_PROGRAM_ID,
    })
  );
  const immutableData = Buffer.alloc(1);
  immutableData.writeUInt8(22, 0);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: account2.publicKey, isSigner: false, isWritable: true },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: immutableData,
    })
  );

  // Token-2022: AmountToUiAmount (disc 23)
  const amountToUiData = Buffer.alloc(9);
  amountToUiData.writeUInt8(23, 0);
  amountToUiData.writeBigUInt64LE(0n, 1);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: mint1.publicKey, isSigner: false, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: amountToUiData,
    })
  );

  // Token-2022: UiAmountToAmount (disc 24)
  const uiAmountData = Buffer.alloc(5);
  uiAmountData.writeUInt8(24, 0);
  uiAmountData.writeFloatLE(0, 1);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: mint1.publicKey, isSigner: false, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: uiAmountData,
    })
  );

  // Token-2022: MemoTransferExtension (disc 30)
  const memoTransferData = Buffer.alloc(2);
  memoTransferData.writeUInt8(30, 0);
  memoTransferData.writeUInt8(1, 1);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: account1.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: memoTransferData,
    })
  );

  // Token-2022: CpiGuardExtension (disc 34)
  const cpiGuardData = Buffer.alloc(2);
  cpiGuardData.writeUInt8(34, 0);
  cpiGuardData.writeUInt8(1, 1);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: account1.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: TOKEN_2022_PROGRAM_ID,
      data: cpiGuardData,
    })
  );

  // Token-2022: GroupMemberPointerExtension (disc 43) - Before InitializeMint2
  const mint2 = Keypair.generate();
  const mintLen2 = getMintLen([ExtensionType.GroupMemberPointer]);
  transaction.add(
    SystemProgram.createAccount({
      fromPubkey: payer.publicKey,
      newAccountPubkey: mint2.publicKey,
      lamports: await connection.getMinimumBalanceForRentExemption(mintLen2),
      space: mintLen2,
      programId: TOKEN_2022_PROGRAM_ID,
    })
  );
  transaction.add(
    createInitializeGroupMemberPointerInstruction(
      mint2.publicKey,
      payer.publicKey,
      mint2.publicKey,
      TOKEN_2022_PROGRAM_ID
    )
  );
  transaction.add(
    createInitializeMintInstruction(
      mint2.publicKey,
      9,
      payer.publicKey,
      payer.publicKey,
      TOKEN_2022_PROGRAM_ID
    )
  );

  // System Program: AdvanceNonceAccount (disc 4)
  const advanceNonceData = Buffer.alloc(4);
  advanceNonceData.writeUInt32LE(4, 0);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: SystemProgram.programId, isSigner: false, isWritable: false },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: SystemProgram.programId,
      data: advanceNonceData,
    })
  );

  // System Program: WithdrawNonceAccount (disc 5)
  const withdrawNonceData = Buffer.alloc(12);
  withdrawNonceData.writeUInt32LE(5, 0);
  withdrawNonceData.writeBigUInt64LE(0n, 4);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: SystemProgram.programId, isSigner: false, isWritable: false },
        { pubkey: payer.publicKey, isSigner: false, isWritable: false },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: SystemProgram.programId,
      data: withdrawNonceData,
    })
  );

  // System Program: AuthorizeNonceAccount (disc 7)
  const authorizeNonceData = Buffer.alloc(36);
  authorizeNonceData.writeUInt32LE(7, 0);
  payer.publicKey.toBuffer().copy(authorizeNonceData, 4);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: SystemProgram.programId,
      data: authorizeNonceData,
    })
  );

  // System Program: TransferWithSeed (disc 11)
  const transferSeedData = Buffer.alloc(52);
  transferSeedData.writeUInt32LE(11, 0);
  transferSeedData.writeBigUInt64LE(0n, 4);
  transferSeedData.writeUInt32LE(4, 12);
  Buffer.from('seed').copy(transferSeedData, 16);
  payer.publicKey.toBuffer().copy(transferSeedData, 20);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
      ],
      programId: SystemProgram.programId,
      data: transferSeedData,
    })
  );

  // Token Program: InitializeMint (disc 0)
  const mint3 = Keypair.generate();
  transaction.add(
    SystemProgram.createAccount({
      fromPubkey: payer.publicKey,
      newAccountPubkey: mint3.publicKey,
      lamports: await connection.getMinimumBalanceForRentExemption(82),
      space: 82,
      programId: new PublicKey('TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA'),
    })
  );
  const initMintData = Buffer.alloc(67);
  initMintData.writeUInt8(0, 0);
  initMintData.writeUInt8(9, 1);
  payer.publicKey.toBuffer().copy(initMintData, 2);
  initMintData.writeUInt8(1, 34);
  payer.publicKey.toBuffer().copy(initMintData, 35);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: mint3.publicKey, isSigner: false, isWritable: true },
        { pubkey: new PublicKey('SysvarRent111111111111111111111111111111111'), isSigner: false, isWritable: false },
      ],
      programId: new PublicKey('TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA'),
      data: initMintData,
    })
  );

  // Token Program: CloseAccount (disc 9)
  const closeTokenData = Buffer.alloc(1);
  closeTokenData.writeUInt8(9, 0);
  transaction.add(
    new TransactionInstruction({
      keys: [
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: false, isWritable: true },
        { pubkey: payer.publicKey, isSigner: true, isWritable: false },
      ],
      programId: new PublicKey('TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA'),
      data: closeTokenData,
    })
  );

  // Compute Budget: RequestUnitsDeprecated (disc 0)
  const computeData = Buffer.alloc(9);
  computeData.writeUInt8(0, 0);
  computeData.writeUInt32LE(200000, 1);
  computeData.writeUInt32LE(1000, 5);
  transaction.add(
    new TransactionInstruction({
      keys: [],
      programId: new PublicKey('ComputeBudget111111111111111111111111111111'),
      data: computeData,
    })
  );

  // Compute Budget: SetLoadedAccountsDataSizeLimit (disc 4)
  const loadedAccountsData = Buffer.alloc(5);
  loadedAccountsData.writeUInt8(4, 0);
  loadedAccountsData.writeUInt32LE(64000, 1);
  transaction.add(
    new TransactionInstruction({
      keys: [],
      programId: new PublicKey('ComputeBudget111111111111111111111111111111'),
      data: loadedAccountsData,
    })
  );

  transaction.partialSign(payer, mint1, account1, account2, mint2, mint3);
  
  return Buffer.from(transaction.serialize()).toString('base64');
}