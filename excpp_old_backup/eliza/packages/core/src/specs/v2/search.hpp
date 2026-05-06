#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Implementation of BM25 and Porter2 stemming
// https://github.com/eilvelia/porter2.js
// https://www.npmjs.com/package/fast-bm25

// The MIT License

// Copyright (c) 2024 eilvelia <hi@eilvelia.cat>

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// MIT License

// Copyright (c) 2024 Vivek Patel <me@patelvivek.dev>.

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * Checks if the character code represents a vowel (a, e, i, o, u, y).
 * @param char - The character code.
 * @returns True if the character is a vowel, false otherwise.
 */

/**
 * Checks if the character code is 'w', 'x', 'y', or a vowel.
 * Used in determining short syllables.
 * @param char - The character code.
 * @returns True if the character is w, x, y, or a vowel, false otherwise.
 */

/**
 * Checks if the character code is one of the valid endings for Step 1c ('li' rule).
 * Valid endings: c, d, e, g, h, k, m, n, r, t.
 * @param char - The character code.
 * @returns True if the character is a valid 'li' ending, false otherwise.
 */

/**
 * Checks if the character code represents a "double" consonant sound
 * (bb, dd, ff, gg, mm, nn, pp, rr, tt).
 * @param char - The character code.
 * @returns True if the character forms a double consonant, false otherwise.
 */

/**
 * Checks if a word ends in a short syllable.
 * A short syllable is defined as:
 * 1. A vowel followed by a non-vowel at the end of the word. (e.g., "hop")
 * 2. A vowel followed by a non-vowel followed by a non-vowel ('w', 'x', 'y' excluded). (e.g., "trap")
 * @param w - Array of character codes representing the word.
 * @param len - The current effective length of the word being considered.
 * @returns True if the word ends in a short syllable, false otherwise.
 */
  // backwardmode: ( non-v_WXY v non-v ) or ( non-v v atlimit )

// #endregion Porter2 Stemmer Helper Functions

// #region Porter2 Stemmer Algorithm

/**
 * Stems a given word using the Porter2 (Snowball English) stemming algorithm.
 *
 * The algorithm works in several steps, applying suffix stripping rules based on
 * regions R1 and R2 within the word.
 * - R1: The region after the first non-vowel following a vowel.
 * - R2: The region after the first non-vowel following a vowel in R1.
 *
 * The steps generally involve:
 * 1. Handling plurals and past participles (-s, -es, -ed, -ing).
 * 2. Turning terminal 'y' to 'i' if there is another vowel in the stem.
 * 3. Mapping double suffixes to single ones (e.g., -ization to -ize).
 * 4. Dealing with suffixes like -full, -ness, etc.
 * 5. Removing suffixes like -ant, -ence, etc.
 * 6. Removing a final -e.
 *
 * @param word - The word to be stemmed.
 * @returns The stemmed version of the word.
 */
  // exception1
      // special -LY cases
      // invariant forms
      // not plural forms
  // mark_regions
  // rv is the position after the first vowel
    // commun
    // > R1 is the region after the first non-vowel following a vowel,
    // > or the end of the word if there is no such non-vowel.
  // > R2 is the region after the first non-vowel following a vowel in R1,
  // > or the end of the word if there is no such non-vowel.
  // Step_1a
      // s
        // sses
        // ies
        // us ss -> <nothing>; s -> "delete if the preceding word part
        // contains a vowel not immediately before the s"
  // exception2
  // Step_1b
    // l (length) without the -ly ending
      // eed
      // ll without: ed edly ing ingly (-1 if not found)
            // at -> ate   bl -> ble   iz -> ize
            // <shortv> -> e
  // Step_1c
  // Step_2
            // a (ational)
              // ational -> ate
            // iz (ization)
              // ization -> ize
            // ation -> ate
            // e (er)
            // o (or)
              // ator
          // c (ic)
            // enci anci
          // g (gi)
            // logi
          // t (ti)
            // iti
              // liti
                // biliti
                  // biliti -> ble
                // aliti
              // iviti
              // iviti -> ive
          // l (li)
            // bli
              // abli
            // Remove li
              // lli
                // fulli
                // alli
              // sli
                // lessli
                // ousli
              // entli
  // Step_3
          // cal
          // ful
          // nal
              // ational
                // ational -> ate
          // ze
          // te
          // ve
  // Step_4
          // nt
            // ant
            // ent
              // ment
                // ement
          // ce
          // le
          // te
          // ive ize
  // Step_5

// #endregion Porter2 Stemmer Algorithm

// src/constants.ts

/**
 * Interface for tokenization statistics.
 */
struct TokenizationStats {
    double originalWordCount;
    double stopWordsRemoved;
    double stemmedWords;
    double processingTimeMs;
};

/**
 * Interface for the result of tokenization.
 */
struct TokenizationResult {
    std::vector<std::string> tokens;
    std::optional<TokenizationStats> stats;
};

/**
 * Interface for stemming rules.
 */
struct StemmingRule {
    RegExp | string pattern;
    string | ((substring: string, ...args: any[]) => string) replacement;
    std::optional<double> minMeasure;
};

/**
 * Options for configuring the Tokenizer.
 */
struct TokenizerOptions {
    std::optional<std::unordered_set<std::string>> stopWords;
    std::optional<double> minLength;
    std::optional<bool> stemming;
    std::optional<std::vector<StemmingRule>> stemmingRules;
};

/**
 * Flexible text tokenizer with support for stop words, minimum token length,
 * Unicode normalization, and optional Porter2 stemming with custom rules.
 */
class Tokenizer {
  /** Set of stop words to ignore. */
  readonly stopWords: Set<string>;
  /** Minimum length of tokens to keep. */
  readonly minLength: number;
  /** Flag indicating if stemming is enabled. */
  readonly stemming: boolean;
  /** Custom stemming rules. */
  readonly stemmingRules: {
    pattern: RegExp;
    replacement: string | ((substring: string, ...args: any[]) => string);
    minMeasure?: number;
  }[];

  /** Default options for the Tokenizer. */

  /**
   * Creates a new tokenizer instance.
   * @param options - Tokenization options including stop words, min length, stemming, and custom rules.
   */
    // Ensure all rule patterns are RegExp objects

  /**
   * Tokenizes input text into an array of processed terms.
   * Steps:
   * 1. Cleans the text (lowercase, normalize, remove punctuation/symbols).
   * 2. Splits the text into potential tokens.
   * 3. Filters tokens based on `minLength` and `stopWords`.
   * 4. Applies stemming if `stemming` is true (custom rules first, then Porter2).
   * 5. Optionally calculates statistics.
   *
   * @param text - The input text string to tokenize.
   * @param includeStats - If true, returns tokenization statistics along with tokens. Defaults to false.
   * @returns A `TokenizationResult` object containing the array of tokens and optional stats.
   * @throws {Error} If the input text is null, undefined, or empty.
   */

  /**
   * Cleans and normalizes text for tokenization.
   * - Converts to lowercase.
   * - Normalizes Unicode characters (NFKD).
   * - Removes control characters and zero-width spaces.
   * - Removes diacritical marks (accents).
   * - Removes emojis and pictographs.
   * - Removes common symbols (™, ®, ©, ℠, ‼).
   * - Replaces Unicode punctuation with spaces.
   * - Removes characters not matching basic Latin, CJK, Hangul, or whitespace.
   * - Collapses multiple spaces into single spaces.
   * - Trims leading/trailing whitespace.
   *
   * @param text - Input text to clean.
   * @returns Cleaned and normalized text, ready for splitting into tokens.
   *
   * @example
   * cleanText("Hello, World™!") // "hello world"
   * cleanText("héllo 👋") // "hello"
   * cleanText("Hello 世界!") // "hello 世界"
   * cleanText("I'm don't") // "i'm don't" (apostrophes kept by replacing punctuation with space)
   * cleanText("test©2023") // "test 2023"
   */

  /**
   * Checks if a token is valid (meets `minLength` criteria and is not a stop word).
   * Numeric tokens are always considered valid regardless of length.
   * @param token - The token string to validate.
   * @returns `true` if the token is valid, `false` otherwise.
   */

  /**
   * Applies stemming to a single word.
   * First, tries to apply custom stemming rules defined in `stemmingRules`.
   * If no custom rule matches, applies the default Porter2 stemming algorithm.
   * Words shorter than 3 characters are not stemmed.
   * @param word - The word to stem.
   * @returns The stemmed word.
   */
          // Apply replacement
            // If replacement is a function, it might need more specific arguments based on its definition.
            // Assuming it takes the matched substring and potentially other match groups.
          // Depending on stemming strategy, might want to break or continue applying rules
    // If a custom rule was applied and modified the word, return it.
    // Otherwise, or if custom rules are meant to precede default stemming, apply Porter2.

    // Fallback to Porter2 if no custom rule applied or if custom rules are pre-processing
    return stem(stemmed); // Apply Porter2 to the (potentially already custom-stemmed) word

  /**
   * Checks if the character at a given index in a word is a consonant.
   * Treats 'y' as a consonant if it's the first letter or follows a consonant.
   * @param word - The word string.
   * @param i - The index of the character to check.
   * @returns `true` if the character is a consonant, `false` otherwise.
   */

  /**
   * Calculates the "measure" of a word stem (approximates syllable count).
   * The measure (m) is the number of times a sequence of vowels is followed by a
   * sequence of consonants (VC). Used in some stemming rules.
   * Example: measure("tree") = 0, measure("trouble") = 1, measure("private") = 2
   * @param word - The word (or stem) to measure.
   * @returns The measure (m) of the word.
   */

/**
 * BM25 Options Interface.
 * Extends TokenizerOptions and adds BM25 specific parameters.
 */
  /**
   * Term frequency saturation parameter (k1). Controls how quickly term frequency
   * saturates. Higher values mean TF contributes more significantly even for high counts.
   * Typical values are between 1.2 and 2.0. Default: 1.2.
   */
  /**
   * Document length normalization parameter (b). Controls the influence of document
   * length. 0 means no length normalization, 1 means full normalization.
   * Typical values are around 0.75. Default: 0.75.
   */
  /**
   * A dictionary defining boost factors for specific document fields.
   * Terms found in fields with higher boost factors will contribute more to the score.
   * Example: `{ title: 2, body: 1 }`. Default: `{}` (no boosts).
   */

/**
 * Represents a search result item.
 */
struct SearchResult {
    double index;
    double score;
    std::optional<any; // Consider using a generic <T> for BM25 class if docs are typed> doc;
};

/**
 * Implements the Okapi BM25 (Best Matching 25) ranking function for information retrieval.
 *
 * BM25 ranks documents based on the query terms appearing in each document,
 * considering term frequency (TF) and inverse document frequency (IDF).
 * It improves upon basic TF-IDF by incorporating:
 * - Term Frequency Saturation (k1): Prevents overly frequent terms from dominating the score.
 * - Document Length Normalization (b): Penalizes documents that are longer than average,
 *   assuming longer documents are more likely to contain query terms by chance.
 *
 * Key Components:
 * - Tokenizer: Processes text into terms (words), handles stop words and stemming.
 * - Document Indexing: Stores document lengths, term frequencies per document,
 *   and overall document frequency for each term.
 * - IDF Calculation: Measures the informativeness of a term based on how many documents contain it.
 * - Scoring: Combines TF, IDF, document length, and parameters k1/b to calculate relevance.
 */
class BM25 {
  /** Term frequency saturation parameter (k1). */
  readonly termFrequencySaturation: number; // k1
  /** Document length normalization factor (b). */
  readonly lengthNormalizationFactor: number; // b
  /** Tokenizer instance used for processing text. */
  readonly tokenizer: Tokenizer;
  /** Array storing the length (number of tokens, adjusted by field boosts) of each document. */
  documentLengths: Uint32Array;
  /** Average length of all documents in the index. */
  averageDocLength: number;
  /** Map from term (string) to its unique integer index. */
  termToIndex: Map<string, number>;
  /** Array storing the document frequency (number of docs containing the term) for each term index. */
  documentFrequency: Uint32Array; // DF for each term index
  /** Map from term index to another map storing `docIndex: termFrequencyInDoc`. */
  termFrequencies: Map<number, Map<number, number>>; // TermIndex -> { DocIndex -> TF }
  /** Boost factors for different fields within documents. */
  /** Array storing the original documents added to the index. */

  /**
   * Creates a new BM25 search instance.
   * @param docs - Optional array of initial documents (objects with string fields) to index.
   * @param options - Configuration options for BM25 parameters (k1, b), tokenizer (stopWords, stemming, minLength), and field boosts.
   */

    // Initialize index structures

    // Index initial documents if provided
      // Assign processed data to instance properties

  /**
   * Processes an array of documents to build the initial index structures.
   * Calculates document lengths, term frequencies, document frequencies, and average document length.
   * @param docs - Array of documents to process.
   * @returns An object containing the calculated index data.
   * @internal
   */

      // Iterate through fields of the document

        // Calculate term frequencies within this field/doc
          // Assign index to new terms

          // Track which documents contain the term

          // Increment frequency for this term in this document

      // Store the calculated length for this document

      // Merge this document's term frequencies into the main structure

    // Calculate document frequency (DF) for each term

  /**
   * Recalculates the average document length based on the current `documentLengths`.
   * @internal
   */
    // Use Array.prototype.reduce for compatibility, though typed array reduce might be faster

  /**
   * Searches the indexed documents for a given query string using the BM25 ranking formula.
   *
   * @param query - The search query text.
   * @param topK - The maximum number of top-scoring results to return. Defaults to 10.
   * @returns An array of `SearchResult` objects, sorted by descending BM25 score.
   */

    // Accumulate scores for each document based on query terms
      // Ignore terms not found in the index

      // Skip terms with non-positive IDF (e.g., term in all docs)

      // Iterate over documents containing this term

        // --- BM25 Term Score Calculation ---
        // Normalizes TF based on document length and saturation parameters.

        // Add the weighted score (IDF * normalized TF) for this term to the document's total score

    // --- Result Generation ---
    // Create result objects, filter out zero scores, sort, and take top K
      // Optionally add: doc: this.getDocument(i) // If you want the full doc in results

  /**
   * Searches for an exact phrase within the indexed documents.
   * Ranks documents containing the exact sequence of tokens higher.
   * Note: This is a basic implementation. More sophisticated phrase search might consider proximity.
   *
   * @param phrase - The exact phrase to search for.
   * @param topK - The maximum number of results to return. Defaults to 10.
   * @returns An array of `SearchResult` objects, sorted by score, for documents containing the phrase.
   */

    // --- Find Candidate Documents ---
    // Start with documents containing the *first* term, then intersect with subsequent terms.

        // First term initializes the candidates
        // Intersect: Keep only documents present in both sets

      // If intersection becomes empty, the phrase cannot exist

    // --- Verify Phrase Occurrence and Score ---

      // Check each field for the phrase

        // Tokenize the field content using the same settings

        // Simple sliding window check for the exact phrase sequence
            // Phrase found! Calculate score for this document based on the phrase terms

    // --- Format and Sort Results ---

  /**
   * Calculates a BM25-like score for a sequence of phrase tokens within a specific document.
   * Sums the individual BM25 scores of the terms in the phrase for that document.
   * @param phraseTokens - The tokenized phrase.
   * @param docIndex - The index of the document to score against.
   * @returns The calculated phrase score for the document.
   * @internal
   */
      // Ignore terms not in index (shouldn't happen if candidate selection worked)

      // Calculate the BM25 contribution of this single term

      // Add IDF * normalized TF to the total phrase score

  /**
   * Adds a single new document to the index.
   * Updates all internal index structures incrementally.
   * Note: For adding many documents, `addDocumentsParallel` is generally more efficient.
   *
   * @param doc - The document object (with string fields) to add.
   * @throws {Error} If the document is null or undefined.
   */

    // --- Update Document List and Lengths ---
    // Resize documentLengths array (simple append)
    // Calculate length later...

    // --- Process Fields and Tokens ---

      // Process each token in the field
        // Add term to index if new

          // Ensure documentFrequency array is large enough
            // Grow exponentially, ensure it's at least termIndex + 1
          // Initialize DF for new term (will be incremented below)

        // Increment frequency for this term in this new document

    // --- Update Global Structures ---
    // Set the calculated length for the new document

    // Add this document's term frequencies to the main map and update DF
      // Add TF entry

      // Increment document frequency for the term
      // Ensure termIndexVal is within bounds of documentFrequency before incrementing
        // This case should ideally not be reached if array was resized correctly

    // Recalculate average document length

  /**
   * Calculates the Inverse Document Frequency (IDF) for a given term index.
   * Uses the BM25 IDF formula: log(1 + (N - n + 0.5) / (n + 0.5))
   * where N is the total number of documents and n is the number of documents
   * containing the term. The +1 smooths the logarithm.
   *
   * @param termIndex - The integer index of the term.
   * @returns The IDF score for the term. Returns 0 if the term is not found or has 0 DF.
   */
    // Ensure termIndex is valid

    // If term appears in 0 documents or more docs than exist (error state), return 0 IDF.

    // Adding 1 inside the log ensures IDF is always non-negative.

  /**
   * Retrieves the term frequency (TF) for a specific term in a specific document.
   * @param termIndex - The integer index of the term.
   * @param docIndex - The index of the document.
   * @returns The term frequency, or 0 if the term is not in the document or indices are invalid.
   */

  /**
   * Retrieves the original document object stored at a given index.
   * @param index - The index of the document to retrieve.
   * @returns The document object.
   * @throws {Error} If the index is out of bounds.
   */
    // Consider using a generic <T>

  /**
   * Clears all indexed documents and resets the BM25 instance to its initial state.
   */

  /**
   * Gets the total number of documents currently indexed.
   * @returns The document count.
   */

  /**
   * Adds multiple documents sequentially by calling `addDocument` for each.
   * This method processes documents sequentially in the main thread.
   * @param docs - An array of documents to add.
   */
    // Allow Promise<void> return type
    // Using Promise.all to potentially run additions concurrently if addDocument becomes async
    // Although the current addDocument is sync, this structure allows future flexibility.
    // Note: If addDocument remains purely synchronous, a simple forEach would also work:
    // docs.forEach(doc => this.addDocument(doc));

} // namespace elizaos
