#!/bin/bash

# Ensure PostgreSQL is running for OTC Trading Desk
# Starts the container if it's not running, waits for it to be healthy

set -e

# Load environment variables from .env.local if it exists
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Helper function to safely source env files (ignores comment and separator lines)
source_env_file() {
  local file="$1"
  if [ ! -f "$file" ]; then
    return 0
  fi
  
  # Temporarily disable exit on error for this function
  set +e
  
  # Filter and export valid environment variables
  # Skip: comments (#), separator lines (only dashes/equals), empty lines
  while IFS= read -r line || [ -n "$line" ]; do
    # Skip empty lines, comments, and separator lines
    if [[ -z "$line" ]] || \
       [[ "$line" =~ ^[[:space:]]*# ]] || \
       [[ "$line" =~ ^[[:space:]]*-+[[:space:]]*$ ]] || \
       [[ "$line" =~ ^[[:space:]]*=+[[:space:]]*$ ]]; then
      continue
    fi
    # Only process lines that look like KEY=VALUE
    if [[ "$line" =~ ^[[:space:]]*[A-Za-z_][A-Za-z0-9_]*= ]]; then
      # Remove leading/trailing whitespace and export
      line=$(echo "$line" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
      export "$line" 2>/dev/null || true
    fi
  done < "$file"
  
  # Re-enable exit on error
  set -e
}

if [ -f "$PROJECT_ROOT/.env.local" ]; then
  source_env_file "$PROJECT_ROOT/.env.local"
fi

if [ -f "$PROJECT_ROOT/.env" ]; then
  source_env_file "$PROJECT_ROOT/.env"
fi

# Check if POSTGRES_URL is set and points to a non-local database
if [ -n "$POSTGRES_URL" ] && [[ ! "$POSTGRES_URL" =~ localhost|127\.0\.0\.1 ]]; then
  echo "✅ Using production database from POSTGRES_URL"
  echo "   Skipping local Docker PostgreSQL setup"
  exit 0
fi

DB_PORT="${VENDOR_OTC_DESK_DB_PORT:-${POSTGRES_DEV_PORT:-5439}}"

echo "🔍 Ensuring PostgreSQL is ready..."

# Check if container exists
if ! docker ps -a --format '{{.Names}}' | grep -q '^otc-postgres$'; then
  echo "📦 PostgreSQL container doesn't exist, creating..."
  cd "$(dirname "$0")/.."
  docker compose -f docker-compose.localnet.yml up -d postgres
else
  # Container exists, check if it's running
  if ! docker ps --format '{{.Names}}' | grep -q '^otc-postgres$'; then
    echo "▶️  Starting PostgreSQL container..."
    docker start otc-postgres
  else
    echo "✅ PostgreSQL container is already running"
  fi
fi

# Wait for database to be ready
echo "⏳ Waiting for PostgreSQL to accept connections..."
RETRIES=0
MAX_RETRIES=30
while [ $RETRIES -lt $MAX_RETRIES ]; do
  if docker exec otc-postgres pg_isready -U eliza -d eliza >/dev/null 2>&1; then
    echo "✅ PostgreSQL is ready on port $DB_PORT!"
    exit 0
  fi
  RETRIES=$((RETRIES+1))
  if [ $RETRIES -eq $MAX_RETRIES ]; then
    echo "❌ Timeout waiting for PostgreSQL"
    echo "💡 Check logs with: docker logs otc-postgres"
    exit 1
  fi
  sleep 1
  if [ $((RETRIES % 5)) -eq 0 ]; then
    echo "  Still waiting... ($RETRIES/$MAX_RETRIES)"
  fi
done

