#!/bin/bash

# ===========================================
# Multifunctional Clock Deployment Script
# ===========================================
# This script prepares the project for GitHub upload by:
# 1. Checking for sensitive files
# 2. Validating Arduino sketch structure
# 3. Running optional tests
# 4. Creating development backups
# 
# Usage: ./deploy.sh [options]
# Options:
#   --check-only    Only validate, don't commit
#   --force         Skip validation warnings
#   --backup        Create backup before deploy

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ARDUINO_DIR="$PROJECT_DIR/firmware/multifunctional-clock"
BACKUP_DIR="$PROJECT_DIR/.backups"

# Parse command line arguments
CHECK_ONLY=false
FORCE_DEPLOY=false
CREATE_BACKUP=false
DEV_MODE=false
USE_COMMIT_HELPER=false

while [[ $# -gt 0 ]]; do
  case $1 in
    --check-only)
      CHECK_ONLY=true
      shift
      ;;
    --force)
      FORCE_DEPLOY=true
      shift
      ;;
    --backup)
      CREATE_BACKUP=true
      shift
      ;;
    --dev)
      DEV_MODE=true
      shift
      ;;
    --interactive)
      USE_COMMIT_HELPER=true
      shift
      ;;
    -h|--help)
      echo "Usage: $0 [--check-only] [--force] [--backup] [--dev] [--interactive]"
      echo "  --check-only    Only validate, don't commit"
      echo "  --force         Skip validation warnings"
      echo "  --backup        Create backup before deploy"
      echo "  --dev           Development mode (ignore some warnings)"
      echo "  --interactive   Use interactive commit helper"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

# Function to print colored output
print_status() {
  echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
  echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
  echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
  echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if we're in a git repository
check_git_repo() {
  if ! git rev-parse --git-dir > /dev/null 2>&1; then
    print_error "Not in a git repository. Run 'git init' first."
    exit 1
  fi
}

# Function to check for sensitive files
check_sensitive_files() {
  print_status "Checking for sensitive files..."
  
  local sensitive_files=(
    "firmware/multifunctional-clock/secrets.h"
    "config_local.h"
    "*.log"
    "serial_output.txt"
  )
  
  local found_sensitive=false
  
  for pattern in "${sensitive_files[@]}"; do
    if find "$PROJECT_DIR" -name "$pattern" -type f 2>/dev/null | grep -q .; then
      print_warning "Found sensitive file: $pattern"
      found_sensitive=true
    fi
  done
  
  # Check if secrets.h exists and is tracked
  if git ls-files | grep -q "secrets.h"; then
    print_error "secrets.h is tracked by git! Add it to .gitignore"
    exit 1
  fi
  
  # Check if secrets.h.template exists
  if [ ! -f "$ARDUINO_DIR/secrets.h.template" ]; then
    print_warning "secrets.h.template not found"
  fi
  
  if $found_sensitive && ! $FORCE_DEPLOY; then
    print_error "Sensitive files found. Use --force to ignore or clean them first."
    exit 1
  fi
}

# Function to validate Arduino sketch structure
validate_arduino_structure() {
  print_status "Validating Arduino sketch structure..."
  
  # Check if Arduino directory exists
  if [ ! -d "$ARDUINO_DIR" ]; then
    print_error "Arduino sketch directory not found: $ARDUINO_DIR"
    exit 1
  fi
  
  # Check if .ino file exists
  local ino_file="$ARDUINO_DIR/multifunctional-clock.ino"
  if [ ! -f "$ino_file" ]; then
    print_error "Main .ino file not found: $ino_file"
    exit 1
  fi
  
  # Check for required header files
  local required_headers=(
    "config.h"
    "ClockManager.h"
    "SensorManager.h"
    "DisplayManager.h"
    "NetworkManager.h"
    "UIManager.h"
  )
  
  for header in "${required_headers[@]}"; do
    if [ ! -f "$ARDUINO_DIR/$header" ]; then
      print_warning "Required header not found: $header"
    fi
  done
  
  print_success "Arduino sketch structure validated"
}

# Function to check code quality
check_code_quality() {
  print_status "Checking code quality..."
  
  # Check for TODO comments
  local todo_count=$(find "$ARDUINO_DIR" -name "*.h" -o -name "*.ino" -o -name "*.cpp" | xargs grep -c "TODO" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')
  if [ "$todo_count" -gt 0 ]; then
    print_warning "Found $todo_count TODO comments"
  fi
  
  # Check for debug mode
  if grep -q "#define DEBUG_MODE.*true" "$ARDUINO_DIR/config.h" 2>/dev/null; then
    if [ "$DEV_MODE" = false ]; then
      print_warning "DEBUG_MODE is enabled - consider disabling for production"
      print_status "  Use --dev flag to suppress this warning during development"
    else
      print_status "DEBUG_MODE enabled (development mode)"
    fi
  fi
  
  # Check file sizes (Arduino has memory constraints)
  local large_files=$(find "$ARDUINO_DIR" -name "*.h" -o -name "*.ino" -o -name "*.cpp" | xargs wc -l | awk '$1 > 500 {print $2 " (" $1 " lines)"}')
  if [ -n "$large_files" ]; then
    print_warning "Large files found (>500 lines):"
    echo "$large_files"
  fi
}

# Function to create backup
create_backup() {
  if $CREATE_BACKUP; then
    print_status "Creating backup..."
    
    local timestamp=$(date +"%Y%m%d_%H%M%S")
    local backup_path="$BACKUP_DIR/backup_$timestamp"
    
    mkdir -p "$backup_path"
    cp -r "$ARDUINO_DIR" "$backup_path/"
    
    print_success "Backup created: $backup_path"
  fi
}

# Function to run tests (if available)
run_tests() {
  print_status "Running tests..."
  
  # Check if Arduino CLI is available for compilation test
  if command -v arduino-cli &> /dev/null; then
    print_status "Attempting compilation test..."
    if arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi "$ARDUINO_DIR" &>/dev/null; then
      print_success "Compilation test passed"
    else
      print_warning "Compilation test failed - check your code"
    fi
  else
    if [ "$DEV_MODE" = false ]; then
      print_warning "Arduino CLI not found - skipping compilation test"
      print_status "  Install Arduino CLI for automatic compilation testing"
    else
      print_status "Arduino CLI not found - skipping compilation test (development mode)"
    fi
  fi
}

# Function to show git status
show_git_status() {
  print_status "Git status:"
  git status --porcelain
  
  print_status "Files to be committed:"
  git diff --cached --name-only
}

# Function to commit and push
commit_and_push() {
  if $CHECK_ONLY; then
    print_success "Check complete - no changes made"
    return
  fi
  
  print_status "Preparing git commit..."
  
  # Add all files except sensitive ones
  git add .
  
  # Show what will be committed
  show_git_status
  
  # Ask for confirmation
  if ! $FORCE_DEPLOY; then
    echo -n "Proceed with commit? (y/N): "
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
      print_status "Deployment cancelled"
      exit 0
    fi
  fi
  
  # Get commit message
  if $USE_COMMIT_HELPER && [ -f "$PROJECT_DIR/commit.sh" ]; then
    print_status "Using interactive commit helper..."
    bash "$PROJECT_DIR/commit.sh"
    return
  else
    echo -n "Enter commit message (or press Enter for default): "
    read -r commit_message
    
    if [ -z "$commit_message" ]; then
      commit_message="Update multifunctional clock - $(date +"%Y-%m-%d %H:%M")"
    fi
  fi
  
  # Commit
  git commit -m "$commit_message"
  
  # Push if remote exists
  if git remote get-url origin &>/dev/null; then
    print_status "Pushing to remote repository..."
    git push
    print_success "Successfully pushed to GitHub"
  else
    print_warning "No remote repository configured"
    print_status "To add GitHub remote:"
    echo "git remote add origin https://github.com/username/multifunctional-clock.git"
    echo "git push -u origin main"
  fi
}

# Main execution
main() {
  print_status "Starting deployment process..."
  
  check_git_repo
  create_backup
  check_sensitive_files
  validate_arduino_structure
  check_code_quality
  run_tests
  commit_and_push
  
  print_success "Deployment process completed!"
}

# Run main function
main "$@"