#!/bin/bash

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

HOST="http://localhost:8080"
NUM_REQUESTS=${1:-50}

echo "======================================"
echo "⚡ Webserv Stress Test"
echo "======================================"
echo ""
echo "Target: $HOST"
echo "Number of requests: $NUM_REQUESTS"
echo ""

# Check if server is running
if ! curl -s "$HOST" > /dev/null 2>&1; then
    echo -e "${RED}Error: Server is not running at $HOST${NC}"
    echo "Please start the server with: ./webserv config/cgi.conf"
    exit 1
fi

echo -e "${GREEN}Server is running!${NC}"
echo ""

# Function to make request and measure time
make_request() {
    local id=$1
    local start=$(date +%s%N)

    response=$(curl -s -w "\n%{http_code}" "$HOST/")
    http_code=$(echo "$response" | tail -n1)

    local end=$(date +%s%N)
    local duration=$(( (end - start) / 1000000 )) # Convert to milliseconds

    if [ "$http_code" == "200" ]; then
        echo -e "Request #$id: ${GREEN}✓${NC} HTTP $http_code (${duration}ms)"
        return 0
    else
        echo -e "Request #$id: ${RED}✗${NC} HTTP $http_code"
        return 1
    fi
}

echo "======================================"
echo "Starting stress test..."
echo "======================================"
echo ""

PASSED=0
FAILED=0
START_TIME=$(date +%s)

# Send requests in parallel
for i in $(seq 1 $NUM_REQUESTS); do
    make_request $i &

    # Limit concurrent requests to avoid overwhelming the system
    if [ $((i % 10)) -eq 0 ]; then
        wait
    fi
done

# Wait for all background jobs to complete
wait

END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

echo ""
echo "======================================"
echo "📊 Results"
echo "======================================"
echo "Total requests: $NUM_REQUESTS"
echo "Duration: ${DURATION}s"
echo "Requests/second: $((NUM_REQUESTS / DURATION))"
echo ""

# Count successes/failures from output
PASSED=$(grep -c "✓" <<< "$(jobs -p | wc -l)")
FAILED=$((NUM_REQUESTS - PASSED))

echo -e "Successful: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"
echo "======================================"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}Stress test passed! 🎉${NC}"
    exit 0
else
    echo -e "${YELLOW}Some requests failed${NC}"
    exit 1
fi
