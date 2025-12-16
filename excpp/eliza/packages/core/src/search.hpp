#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

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
    std::variant<RegExp, std::string> pattern;
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
public:
    Tokenizer(TokenizerOptions = {} options);
    TokenizationResult tokenize(const std::string& text, auto includeStats = false);
    std::string cleanText(const std::string& text);
    bool isValidToken(const std::string& token);
    std::string stemWord(const std::string& word);
    bool isConsonant(const std::string& word, double i);
    double measure(const std::string& word);

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
    std::optional<std::any> doc;
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
public:
    BM25(std::optional<std::vector<std::any>> docs, BM25Options = {} options);
    void recalculateAverageLength();
    std::vector<SearchResult> search(const std::string& query, auto topK = 10);
    std::vector<SearchResult> searchPhrase(const std::string& phrase, auto topK = 10);
    double calculatePhraseScore(const std::vector<std::string>& phraseTokens, double docIndex);
    std::future<void> addDocument(const std::any& doc);
    double calculateIdf(double termIndex);
    double getTermFrequency(double termIndex, double docIndex);
    std::any getDocument(double index);
    void clearDocuments();
    double getDocumentCount();


} // namespace elizaos
