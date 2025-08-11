import React from 'react';
import { render, screen } from '@testing-library/react';
import App from './App';

test('renders hat editor app', () => {
  render(<App />);
  const headingElement = screen.getByText(/Put on your hat/i);
  expect(headingElement).toBeInTheDocument();
});
