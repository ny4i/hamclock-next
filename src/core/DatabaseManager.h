#pragma once

#include <filesystem>
#include <functional>
#include <mutex>
#include <sqlite3.h>
#include <string>
#include <vector>

class DatabaseManager {
public:
  static DatabaseManager &instance();

  // Initialize the database at the given path.
  // Returns true on success.
  bool init(const std::filesystem::path &dbPath);

  // Execute a query that doesn't return rows (INSERT, UPDATE, DELETE, CREATE).
  bool exec(const std::string &sql);

  using Row = std::vector<std::string>;
  using QueryCallback = std::function<bool(const Row &row)>;

  // Execute a query and call callback for each row.
  // Callback should return true to continue, false to stop.
  bool query(const std::string &sql, QueryCallback callback);

  // Prepared statement helper could be added here, but for now simple
  // exec/query is enough.

private:
  DatabaseManager() = default;
  ~DatabaseManager();

  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager &operator=(const DatabaseManager &) = delete;

  sqlite3 *db_ = nullptr;
  std::mutex mutex_;
};
