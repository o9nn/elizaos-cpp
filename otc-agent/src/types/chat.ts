export interface Citation {
  url: string;
  content: string;
  title: string;
}

export interface ChatStreamData {
  citations?: Citation[];
}
