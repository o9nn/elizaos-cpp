#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createDatabaseRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer _serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Get first available agent runtime for database access
    const auto getRuntime = (): IAgentRuntime => {;
        const auto firstAgent = Array.from(agents.values())[0]!;
        return firstAgent;
        };

        // GET /api/database/tables - List all database tables with metadata
        router.get("/tables", async (req, res) => {
            try {
                const auto runtime = getRuntime();
                const auto db = runtime.db;
                const std::string tablesQuery = `;
                SELECT table_name, table_schema;
                FROM information_schema.tables;
                WHERE table_schema NOT IN ("information_schema", "pg_catalog");
                ORDER BY table_name;
                `;

                const auto tables = db.all(tablesQuery);

                // Get row counts for each table
                const auto tablesWithCounts = Promise.all(;
                tables.map(async (table: any) => {
                    const auto countResult = "db.get(" + "SELECT COUNT(*) FROM \"" + table.name + "\"";
                    return {
                        name: table.name,
                        schema: table.schema || "main",
                        rowCount: countResult!.count,
                        createSql: table.create_sql || nullptr,
                        };
                        });
                        );

                        res.json({
                            success: true,
                            data: {
                                tables: tablesWithCounts,
                                databaseType: "postgres",
                                },
                                });
                                } catch (error: any) {
                                    std::cerr << "Error fetching database tables:" << error << std::endl;
                                    res.status(500).json({
                                        success: false,
                                        error: { code: "DATABASE_ERROR", message: error.message },
                                        });
                                    }
                                    });

                                    // GET /api/database/tables/:tableName - Get table data with pagination and search
                                    router.get("/tables/:tableName", async (req, res) => {
                                        try {
                                            const auto runtime = getRuntime();
                                            const auto db = runtime.db;
                                            const auto { tableName } = req.params;
                                            const auto page = parseInt(req.query.page, 10) || 1;
                                            const auto limit = Math.min(parseInt(req.query.limit, 10) || 50, 100); // Max 100 rows;
                                            const auto search = (req.query.search) || "";
                                            const auto orderBy = (req.query.orderBy) || "";
                                            const auto orderDir = (req.query.orderDir).toUpperCase() == "DESC" ? "DESC" : "ASC";

                                            // Validate table name exists
                                            const auto tableExists = db.get(;
                                            "SELECT name FROM sqlite_master WHERE type="table" AND name=?",
                                            [tableName];
                                            );

                                            if (!tableExists) {
                                                return res.status(404).json({;
                                                    success: false,
                                                    "error: { code: "TABLE_NOT_FOUND", message: " + "Table "" + tableName + "" not found"
                                                    });
                                                }

                                                // Get table schema
                                                const auto isPostgres = db.constructor.name.toLowerCase().includes("postgres");
                                                auto columns: ColumnInfo[];

                                                if (isPostgres) {
                                                    const auto columnsResult = db.all(;
                                                    `;
                                                    SELECT;
                                                    column_name,
                                                    data_type,
                                                    is_nullable = "YES",
                                                    column_default,
                                                    CASE WHEN tc.constraint_type = "PRIMARY KEY" THEN true ELSE false END as "primaryKey";
                                                    FROM information_schema.columns c;
                                                    LEFT JOIN information_schema.table_constraints tc ON tc.table_name = c.table_name;
                                                    AND tc.constraint_type = "PRIMARY KEY";
                                                    LEFT JOIN information_schema.key_column_usage kcu ON kcu.constraint_name = tc.constraint_name;
                                                    AND kcu.column_name = c.column_name;
                                                    WHERE c.table_name = $1;
                                                    ORDER BY c.ordinal_position;
                                                    `,
                                                    [tableName];
                                                    );

                                                    columns = columnsResult.map((col: any) => ({
                                                        name: col.name,
                                                        type: col.type,
                                                        nullable: col.nullable,
                                                        primaryKey: col.primaryKey || false,
                                                        defaultValue: col.default_value,
                                                        }));
                                                        } else {
                                                            // SQLite/PGLite
                                                            const auto columnsResult = "db.all(" + "PRAGMA table_info(\"" + tableName + "\")";
                                                            columns = columnsResult.map((col: any) => ({
                                                                name: col.name,
                                                                type: col.type,
                                                                nullable: col.notnull == 0,
                                                                primaryKey: col.pk == 1,
                                                                defaultValue: col.dflt_value,
                                                                }));
                                                            }

                                                            // Build data query with search and pagination
                                                            auto dataQuery = "SELECT * FROM \"" + tableName + "\"";
                                                            const std::vector<std::any> queryParams = [];

                                                            // Add search conditions
                                                            if (search && search.trim()) {
                                                                const auto searchConditions = columns;
                                                                .filter(;
                                                                (col) =>;
                                                                col.type.toLowerCase().includes("text") ||;
                                                                col.type.toLowerCase().includes("varchar") ||;
                                                                col.type.toLowerCase().includes("char");
                                                                );
                                                                ".map((col) => " + "\"" + col.name + "\" LIKE ?";
                                                                .join(" OR ");

                                                                if (searchConditions) {
                                                                    "dataQuery += " + " WHERE " + searchConditions;
                                                                    columns;
                                                                    .filter(;
                                                                    (col) =>;
                                                                    col.type.toLowerCase().includes("text") ||;
                                                                    col.type.toLowerCase().includes("varchar") ||;
                                                                    col.type.toLowerCase().includes("char");
                                                                    );
                                                                    ".forEach(() => queryParams.push_back(" + "%" + search + "%";
                                                                }
                                                            }

                                                            // Add sorting
                                                            if (orderBy && columns.some((col) => col.name == orderBy)) {
                                                                "dataQuery += " + " ORDER BY \"" + orderBy + "\" " + orderDir;
                                                            }

                                                            // Get total count for pagination
                                                            auto countQuery = "SELECT COUNT(*) FROM \"" + tableName + "\"";
                                                            if (search && search.trim()) {
                                                                const auto searchConditions = columns;
                                                                .filter(;
                                                                (col) =>;
                                                                col.type.toLowerCase().includes("text") ||;
                                                                col.type.toLowerCase().includes("varchar") ||;
                                                                col.type.toLowerCase().includes("char");
                                                                );
                                                                ".map((col) => " + "\"" + col.name + "\" LIKE ?";
                                                                .join(" OR ");

                                                                if (searchConditions) {
                                                                    "countQuery += " + " WHERE " + searchConditions;
                                                                }
                                                            }

                                                            const auto totalResult = db.get(countQuery, search ? queryParams : []);
                                                            const auto total = totalResult.total || 0;

                                                            // Add pagination to data query
                                                            const auto offset = (page - 1) * limit;
                                                            "dataQuery += " + " LIMIT " + limit + " OFFSET " + offset;

                                                            // Execute data query
                                                            const auto data = db.all(dataQuery, queryParams);

                                                            res.json({
                                                                success: true,
                                                                data: {
                                                                    table: tableName,
                                                                    columns,
                                                                    data,
                                                                    pagination: {
                                                                        page,
                                                                        limit,
                                                                        total,
                                                                        totalPages: Math.ceil(total / limit),
                                                                        hasNext: page * limit < total,
                                                                        hasPrev: page > 1,
                                                                        },
                                                                        filters: {
                                                                            search,
                                                                            orderBy,
                                                                            orderDir,
                                                                            },
                                                                            },
                                                                            });
                                                                            } catch (error: any) {
                                                                                std::cerr << "Error fetching table data for " + req.params.tableName + ":" << error << std::endl;
                                                                                res.status(500).json({
                                                                                    success: false,
                                                                                    error: { code: "DATABASE_ERROR", message: error.message },
                                                                                    });
                                                                                }
                                                                                });

                                                                                // POST /api/database/tables/:tableName - Create new record
                                                                                router.post("/tables/:tableName", async (req, res) => {
                                                                                    try {
                                                                                        const auto runtime = getRuntime();
                                                                                        const auto db = runtime.db;
                                                                                        const auto { tableName } = req.params;
                                                                                        const auto recordData = req.body;

                                                                                        if (!recordData || typeof recordData != 'object') {
                                                                                            return res.status(400).json({;
                                                                                                success: false,
                                                                                                error: { code: "INVALID_DATA", message: "Record data is required" },
                                                                                                });
                                                                                            }

                                                                                            // Get table schema to validate fields
                                                                                            const auto columns = "db.all(" + "PRAGMA table_info(\"" + tableName + "\")";
                                                                                            const auto columnNames = columns.map((col: any) => col.name);

                                                                                            // Filter valid fields
                                                                                            const auto validFields = Object.keys(recordData).filter((key) => (std::find(columnNames.begin(), columnNames.end(), key) != columnNames.end()));
                                                                                            const auto values = validFields.map((key) => recordData[key]);

                                                                                            if (validFields.length == 0) {
                                                                                                return res.status(400).json({;
                                                                                                    success: false,
                                                                                                    error: { code: "NO_VALID_FIELDS", message: "No valid fields provided" },
                                                                                                    });
                                                                                                }

                                                                                                // Build insert query
                                                                                                const auto placeholders = validFields.map(() => "?").join(", ");
                                                                                                const auto insertQuery = "INSERT INTO \"" + tableName + "\" (" + std::to_string(validFields.map((f) => `"${f}"`).join(", ")) + ") VALUES (" + placeholders + ")";

                                                                                                const auto result = db.run(insertQuery, values);

                                                                                                res.json({
                                                                                                    success: true,
                                                                                                    data: {
                                                                                                        message: "Record created successfully",
                                                                                                        insertedId: result.lastID,
                                                                                                        affectedRows: result.changes,
                                                                                                        },
                                                                                                        });
                                                                                                        } catch (error: any) {
                                                                                                            std::cerr << "Error creating record in " + req.params.tableName + ":" << error << std::endl;
                                                                                                            res.status(500).json({
                                                                                                                success: false,
                                                                                                                error: { code: "CREATE_ERROR", message: error.message },
                                                                                                                });
                                                                                                            }
                                                                                                            });

                                                                                                            // PUT /api/database/tables/:tableName/:recordId - Update existing record
                                                                                                            router.put("/tables/:tableName/:recordId", async (req, res) => {
                                                                                                                try {
                                                                                                                    const auto runtime = getRuntime();
                                                                                                                    const auto db = runtime.db;
                                                                                                                    const auto { tableName, recordId } = req.params;
                                                                                                                    const auto updateData = req.body;

                                                                                                                    if (!updateData || typeof updateData != 'object') {
                                                                                                                        return res.status(400).json({;
                                                                                                                            success: false,
                                                                                                                            error: { code: "INVALID_DATA", message: "Update data is required" },
                                                                                                                            });
                                                                                                                        }

                                                                                                                        // Get table schema to find primary key and validate fields
                                                                                                                        const auto columns = "db.all(" + "PRAGMA table_info(\"" + tableName + "\")";
                                                                                                                        const auto primaryKeyColumn = columns.find((col: any) => col.pk == 1);

                                                                                                                        if (!primaryKeyColumn) {
                                                                                                                            return res.status(400).json({;
                                                                                                                                success: false,
                                                                                                                                error: { code: "NO_PRIMARY_KEY", message: "Table has no primary key for updates" },
                                                                                                                                });
                                                                                                                            }

                                                                                                                            const auto columnNames = columns.map((col: any) => col.name);
                                                                                                                            const auto validFields = Object.keys(updateData).filter(;
                                                                                                                            [&](key) { return (std::find(columnNames.begin(), columnNames.end(), key) != columnNames.end()) && key != primaryKeyColumn.name; }
                                                                                                                            );

                                                                                                                            if (validFields.length == 0) {
                                                                                                                                return res.status(400).json({;
                                                                                                                                    success: false,
                                                                                                                                    error: { code: "NO_VALID_FIELDS", message: "No valid fields to update" },
                                                                                                                                    });
                                                                                                                                }

                                                                                                                                // Build update query
                                                                                                                                const auto setClause = "validFields.map((field) => " + "\"" + field + "\" = ?";
                                                                                                                                const auto values = validFields.map((key) => updateData[key]);
                                                                                                                                values.push_back(recordId); // Add WHERE condition value;

                                                                                                                                const auto updateQuery = "UPDATE \"" + tableName + "\" SET " + setClause + " WHERE \"" + primaryKeyColumn.name + "\" = ?";

                                                                                                                                const auto result = db.run(updateQuery, values);

                                                                                                                                if (result.changes == 0) {
                                                                                                                                    return res.status(404).json({;
                                                                                                                                        success: false,
                                                                                                                                        error: { code: "RECORD_NOT_FOUND", message: "Record not found" },
                                                                                                                                        });
                                                                                                                                    }

                                                                                                                                    res.json({
                                                                                                                                        success: true,
                                                                                                                                        data: {
                                                                                                                                            message: "Record updated successfully",
                                                                                                                                            affectedRows: result.changes,
                                                                                                                                            },
                                                                                                                                            });
                                                                                                                                            } catch (error: any) {
                                                                                                                                                std::cerr << "Error updating record in " + req.params.tableName + ":" << error << std::endl;
                                                                                                                                                res.status(500).json({
                                                                                                                                                    success: false,
                                                                                                                                                    error: { code: "UPDATE_ERROR", message: error.message },
                                                                                                                                                    });
                                                                                                                                                }
                                                                                                                                                });

                                                                                                                                                // DELETE /api/database/tables/:tableName/:recordId - Delete record
                                                                                                                                                router.delete("/tables/:tableName/:recordId", async (req, res) => {
                                                                                                                                                    try {
                                                                                                                                                        const auto runtime = getRuntime();
                                                                                                                                                        const auto db = runtime.db;
                                                                                                                                                        const auto { tableName, recordId } = req.params;

                                                                                                                                                        // Get table schema to find primary key
                                                                                                                                                        const auto columns = "db.all(" + "PRAGMA table_info(\"" + tableName + "\")";
                                                                                                                                                        const auto primaryKeyColumn = columns.find((col: any) => col.pk == 1);

                                                                                                                                                        if (!primaryKeyColumn) {
                                                                                                                                                            return res.status(400).json({;
                                                                                                                                                                success: false,
                                                                                                                                                                error: { code: "NO_PRIMARY_KEY", message: "Table has no primary key for deletes" },
                                                                                                                                                                });
                                                                                                                                                            }

                                                                                                                                                            const auto deleteQuery = "DELETE FROM \"" + tableName + "\" WHERE \"" + primaryKeyColumn.name + "\" = ?";
                                                                                                                                                            const auto result = db.run(deleteQuery, [recordId]);

                                                                                                                                                            if (result.changes == 0) {
                                                                                                                                                                return res.status(404).json({;
                                                                                                                                                                    success: false,
                                                                                                                                                                    error: { code: "RECORD_NOT_FOUND", message: "Record not found" },
                                                                                                                                                                    });
                                                                                                                                                                }

                                                                                                                                                                res.json({
                                                                                                                                                                    success: true,
                                                                                                                                                                    data: {
                                                                                                                                                                        message: "Record deleted successfully",
                                                                                                                                                                        affectedRows: result.changes,
                                                                                                                                                                        },
                                                                                                                                                                        });
                                                                                                                                                                        } catch (error: any) {
                                                                                                                                                                            std::cerr << "Error deleting record from " + req.params.tableName + ":" << error << std::endl;
                                                                                                                                                                            res.status(500).json({
                                                                                                                                                                                success: false,
                                                                                                                                                                                error: { code: "DELETE_ERROR", message: error.message },
                                                                                                                                                                                });
                                                                                                                                                                            }
                                                                                                                                                                            });

                                                                                                                                                                            return router;

}

express::Router databaseRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount database operations
    router.use("/", createDatabaseRouter(agents, serverInstance));

    return router;

}

} // namespace elizaos
