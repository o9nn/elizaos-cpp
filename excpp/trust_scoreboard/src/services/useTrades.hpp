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


type TTransaction = {
  hash: string
  tokenAddr: string
  tokenName: string
  amount: string
  type: "sell" | "buy"
  uiAmount: string
}

type TTrade = {
  id: number
  tokenAddr: string
  tokenName: string
  logoURI?: string
  decimals: number
  bundleSignature: string
  amount: string
  createdAt: string
  updatedAt: string
  transactions: TTransaction[]
  uiAmount: string
}

const useGetTrades = () => {
  const [isLoading, setIsLoading] = useState(true)
  const [trades, setTrades] = useState<TTrade[]>([])

  const getUsers = async () => {
    const response = await fetch(`/api/trades/getDaoTrades`)

    setIsLoading(false)
    const data = await response.json()

    setTrades(data)
  }

  useEffect(() => {
    getUsers()
  }, [])

  return { trades, isLoading }
}

} // namespace elizaos
