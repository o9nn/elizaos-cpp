/** @type {import('tailwindcss').Config} */
module.exports = {
    content: [
      './pages/**/*.{js,ts,jsx,tsx,mdx}',
      './components/**/*.{js,ts,jsx,tsx,mdx}',
      './app/**/*.{js,ts,jsx,tsx,mdx}',
      './src/**/*.{js,ts,jsx,tsx,mdx}',
    ],
    theme: {
      extend: {
        colors: {
          // Brand orange palette - primary accent color
          brand: {
            50: '#fff7ed',
            100: '#ffedd5',
            200: '#fed7aa',
            300: '#FFB79B',  // Light peach - used for borders
            400: '#fb923c',
            500: '#F75B1E',  // Primary brand orange
            600: '#ea580c',
            700: '#c2410c',
            800: '#9a3412',
            900: '#7c2d12',
            950: '#431407',
          },
          // Dark background palette
          surface: {
            DEFAULT: '#020818',
            50: '#f8fafc',
            100: '#f1f5f9',
            200: '#e2e8f0',
            700: '#334155',
            800: '#1e293b',
            900: '#0f172a',
            950: '#020818',  // Primary dark background
          },
          // Chain-specific colors
          solana: {
            purple: '#9945FF',
            green: '#14F195',
          },
          coinbase: {
            blue: '#0052ff',
          },
        },
      },
    },
    plugins: [
      require('@tailwindcss/typography'),
    ],
  }
