#!/bin/bash

# ===========================================
# Multifunctional Clock Commit Helper
# ===========================================
# This script helps create well-structured commits following
# conventional commit format and project standards.

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

print_header() {
  echo -e "${BLUE}================================================${NC}"
  echo -e "${BLUE}    Multifunctional Clock - Commit Helper${NC}"
  echo -e "${BLUE}================================================${NC}"
  echo ""
}

print_status() {
  echo -e "${CYAN}[INFO]${NC} $1"
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

# Function to show current git status
show_git_status() {
  print_status "Current git status:"
  echo ""
  
  # Show branch
  local branch=$(git branch --show-current)
  echo -e "📍 Current branch: ${GREEN}$branch${NC}"
  
  # Show status
  if [[ -n $(git status --porcelain) ]]; then
    echo -e "📝 Modified files:"
    git status --short
  else
    echo -e "✅ Working directory clean"
  fi
  echo ""
}

# Function to get commit type
get_commit_type() {
  echo -e "${YELLOW}Select commit type:${NC}"
  echo "1) feat     - New feature"
  echo "2) fix      - Bug fix"
  echo "3) docs     - Documentation"
  echo "4) refactor - Code refactoring"
  echo "5) test     - Tests"
  echo "6) chore    - Maintenance/config"
  echo "7) style    - Code formatting"
  echo ""
  
  while true; do
    read -p "Enter choice (1-7): " choice
    case $choice in
      1) echo "feat"; break;;
      2) echo "fix"; break;;
      3) echo "docs"; break;;
      4) echo "refactor"; break;;
      5) echo "test"; break;;
      6) echo "chore"; break;;
      7) echo "style"; break;;
      *) echo "Invalid choice. Please enter 1-7.";;
    esac
  done
}

# Function to get commit scope
get_commit_scope() {
  echo -e "${YELLOW}Select scope (optional):${NC}"
  echo "1) clock    - Clock/time functionality"
  echo "2) sensors  - Sensor management"
  echo "3) display  - Display/LED management"
  echo "4) network  - WiFi/network features"
  echo "5) ui       - User interface"
  echo "6) config   - Configuration"
  echo "7) hardware - Hardware setup"
  echo "8) none     - No specific scope"
  echo ""
  
  while true; do
    read -p "Enter choice (1-8): " choice
    case $choice in
      1) echo "clock"; break;;
      2) echo "sensors"; break;;
      3) echo "display"; break;;
      4) echo "network"; break;;
      5) echo "ui"; break;;
      6) echo "config"; break;;
      7) echo "hardware"; break;;
      8) echo ""; break;;
      *) echo "Invalid choice. Please enter 1-8.";;
    esac
  done
}

# Function to validate commit message
validate_commit_message() {
  local message="$1"
  
  # Check length
  if [[ ${#message} -lt 10 ]]; then
    print_error "Commit message too short (minimum 10 characters)"
    return 1
  fi
  
  if [[ ${#message} -gt 72 ]]; then
    print_warning "Commit message quite long (>72 characters). Consider shortening."
  fi
  
  # Check for imperative mood (basic check)
  if [[ $message =~ ^(added|fixed|updated|changed) ]]; then
    print_warning "Consider using imperative mood (add, fix, update, change)"
  fi
  
  return 0
}

# Function to suggest files to add
suggest_files_to_add() {
  local modified_files=$(git status --porcelain | grep '^[ MARC]M\|^A[ MD]\|^??')
  
  if [[ -n "$modified_files" ]]; then
    echo -e "${YELLOW}Files to potentially add:${NC}"
    echo "$modified_files"
    echo ""
    
    echo "Options:"
    echo "1) Add all files"
    echo "2) Add specific files"
    echo "3) Show diff first"
    echo "4) Cancel"
    echo ""
    
    read -p "Enter choice (1-4): " choice
    case $choice in
      1)
        git add .
        print_success "All files added"
        ;;
      2)
        echo "Enter filenames separated by spaces:"
        read -r files
        git add $files
        print_success "Selected files added"
        ;;
      3)
        git diff --cached
        echo ""
        read -p "Proceed with commit? (y/N): " proceed
        if [[ ! $proceed =~ ^[Yy]$ ]]; then
          print_status "Commit cancelled"
          exit 0
        fi
        ;;
      4)
        print_status "Commit cancelled"
        exit 0
        ;;
    esac
  else
    print_warning "No modified files to commit"
    exit 1
  fi
}

# Function to create and execute commit
create_commit() {
  local commit_type="$1"
  local commit_scope="$2"
  local commit_description="$3"
  local commit_body="$4"
  
  # Build commit message
  local commit_message="$commit_type"
  if [[ -n "$commit_scope" ]]; then
    commit_message="$commit_message($commit_scope)"
  fi
  commit_message="$commit_message: $commit_description"
  
  if [[ -n "$commit_body" ]]; then
    commit_message="$commit_message

$commit_body"
  fi
  
  # Show final commit message
  echo -e "${YELLOW}Final commit message:${NC}"
  echo "─────────────────────────"
  echo "$commit_message"
  echo "─────────────────────────"
  echo ""
  
  # Confirm commit
  read -p "Proceed with this commit? (y/N): " confirm
  if [[ $confirm =~ ^[Yy]$ ]]; then
    git commit -m "$commit_message"
    print_success "Commit created successfully!"
    
    # Ask about pushing
    echo ""
    read -p "Push to remote repository? (y/N): " push_confirm
    if [[ $push_confirm =~ ^[Yy]$ ]]; then
      local current_branch=$(git branch --show-current)
      git push origin "$current_branch"
      print_success "Changes pushed to origin/$current_branch"
    fi
  else
    print_status "Commit cancelled"
  fi
}

# Main function
main() {
  print_header
  
  # Check if we're in a git repository
  if ! git rev-parse --git-dir > /dev/null 2>&1; then
    print_error "Not in a git repository"
    exit 1
  fi
  
  show_git_status
  
  # Check if there are changes to commit
  if [[ -z $(git status --porcelain) ]]; then
    print_success "Working directory clean - nothing to commit"
    exit 0
  fi
  
  # Suggest files to add
  suggest_files_to_add
  
  # Get commit information
  local commit_type=$(get_commit_type)
  local commit_scope=$(get_commit_scope)
  
  echo ""
  echo -e "${YELLOW}Enter commit description:${NC}"
  echo "  (Brief description of what this commit does)"
  read -p "Description: " commit_description
  
  # Validate commit description
  if ! validate_commit_message "$commit_description"; then
    exit 1
  fi
  
  echo ""
  echo -e "${YELLOW}Enter detailed description (optional):${NC}"
  echo "  (Press Enter to skip, or enter multi-line description)"
  echo "  (End with a single . on its own line)"
  commit_body=""
  while IFS= read -r line; do
    if [[ "$line" == "." ]]; then
      break
    fi
    if [[ -n "$commit_body" ]]; then
      commit_body="$commit_body
$line"
    else
      commit_body="$line"
    fi
  done
  
  # Create the commit
  create_commit "$commit_type" "$commit_scope" "$commit_description" "$commit_body"
}

# Check for help flag
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
  echo "Usage: $0"
  echo ""
  echo "Interactive commit helper for Multifunctional Clock project"
  echo "Follows conventional commit format: type(scope): description"
  echo ""
  echo "Examples of generated commits:"
  echo "  feat(clock): add LED hour transition animation"
  echo "  fix(sensors): correct DHT22 temperature reading"
  echo "  docs(readme): update installation instructions"
  echo ""
  exit 0
fi

# Run main function
main "$@"