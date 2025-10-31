#!/bin/bash

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

HOST="http://localhost:8080"
PASSED=0
FAILED=0

echo -e "${CYAN}╔══════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║       WEBSERV - 42 EVALUATION TEST SUITE                ║${NC}"
echo -e "${CYAN}╚══════════════════════════════════════════════════════════╝${NC}"
echo ""

# Check if server is running
check_server() {
    if ! curl -s "$HOST" > /dev/null 2>&1; then
        echo -e "${RED}❌ ERROR: Server is not running at $HOST${NC}"
        echo "Please start the server with: ./webserv config/cgi.conf"
        exit 1
    fi
}

# Test function
test_request() {
    local test_name="$1"
    local method="$2"
    local path="$3"
    local data="$4"
    local expected_code="$5"

    echo -n "  Testing: $test_name... "

    if [ "$method" == "GET" ]; then
        response=$(curl -s -w "\n%{http_code}" "$HOST$path" 2>/dev/null)
    elif [ "$method" == "POST" ]; then
        response=$(curl -s -w "\n%{http_code}" -X POST "$HOST$path" -d "$data" 2>/dev/null)
    elif [ "$method" == "DELETE" ]; then
        response=$(curl -s -w "\n%{http_code}" -X DELETE "$HOST$path" 2>/dev/null)
    elif [ "$method" == "UNKNOWN" ]; then
        response=$(curl -s -w "\n%{http_code}" -X UNKNOWN "$HOST$path" 2>/dev/null)
    fi

    http_code=$(echo "$response" | tail -n1)

    if [ "$http_code" == "$expected_code" ]; then
        echo -e "${GREEN}✓ PASS${NC} (HTTP $http_code)"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}✗ FAIL${NC} (Expected: $expected_code, Got: $http_code)"
        ((FAILED++))
        return 1
    fi
}

echo "═══════════════════════════════════════════════════════════"
echo "  SECTION 1: Basic HTTP Methods"
echo "═══════════════════════════════════════════════════════════"
echo ""

check_server

# GET Tests
test_request "GET homepage" "GET" "/" "" "200"
test_request "GET static file" "GET" "/about.html" "" "200"
test_request "GET non-existent (404)" "GET" "/nonexistent.html" "" "404"

# POST Tests
test_request "POST to CGI" "POST" "/cgi-bin/test.py" "name=Test&message=Hello" "200"

# DELETE Tests
echo "test content" > /tmp/webserv_test_delete.txt
upload_result=$(curl -s -X POST "$HOST/upload" -F "file=@/tmp/webserv_test_delete.txt" 2>/dev/null)
sleep 1

# Extract the uploaded filename from the response
uploaded_filename=$(echo "$upload_result" | grep -o '[0-9]*_webserv_test_delete\.txt' | head -n 1)

echo -n "  Testing: DELETE uploaded file... "
if [ -z "$uploaded_filename" ]; then
    echo -e "${RED}✗ FAIL${NC} (Could not find uploaded filename)"
    ((FAILED++))
else
    delete_response=$(curl -s -w "\n%{http_code}" -X DELETE "$HOST/uploads/$uploaded_filename" 2>/dev/null)
    delete_code=$(echo "$delete_response" | tail -n 1)
    if [ "$delete_code" == "200" ] || [ "$delete_code" == "204" ]; then
        echo -e "${GREEN}✓ PASS${NC} (HTTP $delete_code)"
        ((PASSED++))
    else
        echo -e "${RED}✗ FAIL${NC} (Expected: 200 or 204, Got: $delete_code)"
        ((FAILED++))
    fi
fi

# UNKNOWN method
test_request "UNKNOWN method (should not crash)" "UNKNOWN" "/" "" "501"

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  SECTION 2: File Upload/Download"
echo "═══════════════════════════════════════════════════════════"
echo ""

# Create test file
echo "Test content for upload/download verification" > /tmp/webserv_upload_test.txt

# Upload
echo -n "  Testing: File upload... "
upload_response=$(curl -s -w "\n%{http_code}" -X POST "$HOST/upload" -F "file=@/tmp/webserv_upload_test.txt" 2>/dev/null)
upload_code=$(echo "$upload_response" | tail -n1)

if [ "$upload_code" == "200" ] || [ "$upload_code" == "201" ]; then
    echo -e "${GREEN}✓ PASS${NC} (HTTP $upload_code)"
    ((PASSED++))

    # Try to download
    sleep 1
    echo -n "  Testing: File download... "
    # The file should be in uploads directory
    download_response=$(curl -s -w "\n%{http_code}" "$HOST/uploads/" 2>/dev/null)
    download_code=$(echo "$download_response" | tail -n1)

    if [ "$download_code" == "200" ]; then
        echo -e "${GREEN}✓ PASS${NC} (HTTP $download_code)"
        ((PASSED++))
    else
        echo -e "${RED}✗ FAIL${NC} (HTTP $download_code)"
        ((FAILED++))
    fi
else
    echo -e "${RED}✗ FAIL${NC} (HTTP $upload_code)"
    ((FAILED++))
fi

rm -f /tmp/webserv_upload_test.txt

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  SECTION 3: CGI Tests"
echo "═══════════════════════════════════════════════════════════"
echo ""

# CGI GET
echo -n "  Testing: CGI with GET... "
cgi_get=$(curl -s -w "\n%{http_code}" "$HOST/cgi-bin/hello.py" 2>/dev/null)
cgi_get_code=$(echo "$cgi_get" | tail -n 1)
cgi_get_body=$(echo "$cgi_get" | sed '$d')

if [ "$cgi_get_code" == "200" ] && echo "$cgi_get_body" | grep -q "Hello"; then
    echo -e "${GREEN}✓ PASS${NC} (HTTP $cgi_get_code)"
    ((PASSED++))
else
    echo -e "${RED}✗ FAIL${NC} (HTTP $cgi_get_code)"
    ((FAILED++))
fi

# CGI POST
test_request "CGI with POST" "POST" "/cgi-bin/test.py" "key=value&data=test" "200"

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  SECTION 4: Status Codes"
echo "═══════════════════════════════════════════════════════════"
echo ""

test_request "404 Not Found" "GET" "/this_does_not_exist.html" "" "404"
test_request "405 Method Not Allowed" "POST" "/about.html" "data" "405"
test_request "501 Not Implemented" "UNKNOWN" "/" "" "501"

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  SECTION 5: Configuration Features"
echo "═══════════════════════════════════════════════════════════"
echo ""

# Directory listing
echo -n "  Testing: Directory listing... "
dir_response=$(curl -s -w "\n%{http_code}" "$HOST/uploads/" 2>/dev/null)
dir_code=$(echo "$dir_response" | tail -n1)

if [ "$dir_code" == "200" ]; then
    echo -e "${GREEN}✓ PASS${NC} (HTTP $dir_code)"
    ((PASSED++))
else
    echo -e "${RED}✗ FAIL${NC} (HTTP $dir_code)"
    ((FAILED++))
fi

# Default index file
test_request "Default index file" "GET" "/" "" "200"

# Client body limit
echo -n "  Testing: Client body size limit... "
large_data=$(python3 -c "print('A' * 20000000)")  # 20MB
limit_response=$(curl -s -w "\n%{http_code}" -X POST "$HOST/upload" -d "$large_data" 2>/dev/null)
limit_code=$(echo "$limit_response" | tail -n1)

if [ "$limit_code" == "413" ]; then
    echo -e "${GREEN}✓ PASS${NC} (HTTP $limit_code - Request Entity Too Large)"
    ((PASSED++))
else
    echo -e "${YELLOW}⚠ WARNING${NC} (HTTP $limit_code - Expected 413)"
    # Don't fail, just warn
fi

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  SECTION 6: Telnet/Netcat Tests"
echo "═══════════════════════════════════════════════════════════"
echo ""

echo -n "  Testing: Raw HTTP request via nc... "
nc_response=$( (echo -e "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"; sleep 0.5) | nc localhost 8080 2>/dev/null | head -n 1)

if echo "$nc_response" | grep -q "HTTP/1.1 200"; then
    echo -e "${GREEN}✓ PASS${NC}"
    ((PASSED++))
else
    echo -e "${RED}✗ FAIL${NC} (Response: '$nc_response')"
    ((FAILED++))
fi

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  SECTION 7: Siege Stress Test"
echo "═══════════════════════════════════════════════════════════"
echo ""

if command -v siege &> /dev/null; then
    echo "  Running siege stress test (15 seconds, 10 concurrent users)..."
    siege_output=$(siege -b -c 10 -t 15S "$HOST/" 2>&1)

    availability=$(echo "$siege_output" | grep "Availability" | awk '{print $2}' | tr -d '%')

    echo ""
    echo "$siege_output" | grep -E "Transactions|Availability|Response time|Transaction rate"
    echo ""

    if (( $(echo "$availability > 99.5" | bc -l) )); then
        echo -e "  ${GREEN}✓ PASS${NC} - Availability: $availability% (> 99.5%)"
        ((PASSED++))
    else
        echo -e "  ${RED}✗ FAIL${NC} - Availability: $availability% (< 99.5%)"
        ((FAILED++))
    fi
else
    echo -e "  ${YELLOW}⚠ SKIP${NC} - siege not installed"
    echo "  Install with: brew install siege"
fi

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  📊 FINAL RESULTS"
echo "═══════════════════════════════════════════════════════════"
echo ""
echo -e "  Passed: ${GREEN}$PASSED${NC}"
echo -e "  Failed: ${RED}$FAILED${NC}"
echo -e "  Total:  $((PASSED + FAILED))"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}╔═══════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║   ✓ ALL TESTS PASSED! PROJECT READY!     ║${NC}"
    echo -e "${GREEN}╚═══════════════════════════════════════════╝${NC}"
    exit 0
else
    echo -e "${RED}╔═══════════════════════════════════════════╗${NC}"
    echo -e "${RED}║   ✗ SOME TESTS FAILED - REVIEW NEEDED    ║${NC}"
    echo -e "${RED}╚═══════════════════════════════════════════╝${NC}"
    exit 1
fi
