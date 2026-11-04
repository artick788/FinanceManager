# FinanceManager
Manage your finances with this handy app!

## Features
- **Dashboard**: this month's income, expenses and what is left over, 12 month averages, savings rate, cash flow and
  savings charts and a breakdown of your fixed costs
- **Income**: every paycheck per income source, grouped per year with yearly totals
- **Savings**: balance vs. expected balance per account, with yearly growth, gains and a trend line
- **Monthly expenses**: one row per month, one column per expense, missing months are filled in automatically
- All tables show the newest data first, amounts are edited in place by clicking them, right click a row to delete it
- Optionally sync your data with a git repository (File > Settings), so it stays the same on every PC

## Shortcuts
| Shortcut           | Action                     |
|--------------------|----------------------------|
| Ctrl+1 ... Ctrl+5  | Dashboard, Income, Savings, Monthly expenses, Manage |
| Ctrl+P / E / M     | Add a paycheck / savings entry / monthly expense entry |
| Ctrl+S             | Save (and sync with git if enabled) |
| Ctrl+Shift+S       | Sync with git              |
| Ctrl+B / Ctrl+G    | Toggle the sidebar / charts |
| Ctrl+,             | Settings                   |

## Data
Data is stored in `~/.config/FinanceManager/settings_FinanceManager.json` under the `Store` key, in the same format as
before. Files written by versions before VelyraAppFramework v0.4 (data under `AppSettings`/`Store`) are imported
automatically on the first start, the original file is kept as `settings_FinanceManager.pre-v0.4-backup.json`.

## Usage
1. Clone the repository:
   ```bash
   git clone git@github.com:artick788/FinanceManager.git
   ```
2. Navigate to the project directory:
   ```bash
   cd FinanceManager
   ```
3. Run install script
   This will build the app from source and install it on your system.
   ```bash
   ./chmod +x install.sh
   ./install.sh
   ```
4. Launch the app from the terminal:
   ```bash
   FinanceManagerApp
   ```
   If the app does not launch, you might need to add it to your PATH:
   ```bash
   export PATH="$HOME/.local/bin:$PATH"
   ```
   Consider adding this to your bashrc or zshrc file to make it permanent.
