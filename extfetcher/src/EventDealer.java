//import com.alibaba.otter.canal.protocol.CanalEntry;

import java.io.*;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicLong;

import com.alibaba.druid.sql.ast.SQLStatement;
import com.alibaba.druid.sql.ast.statement.SQLAlterTableStatement;
import com.alibaba.druid.sql.ast.statement.SQLCreateDatabaseStatement;
import com.alibaba.druid.sql.ast.statement.SQLCreateTableStatement;
import com.alibaba.druid.sql.ast.statement.SQLDropDatabaseStatement;
import com.taobao.tddl.dbsync.binlog.LogEvent;
import com.taobao.tddl.dbsync.binlog.event.QueryLogEvent;
import com.taobao.tddl.dbsync.binlog.event.RowsLogEvent;
import org.apache.commons.lang3.tuple.ImmutablePair;

public class EventDealer {
    Path binlogFolder;
    String index;
    HashMap<ImmutablePair<String, String>, Integer> tableAlterStateLookup = new HashMap<>();
    HashMap<ImmutablePair<String, String>, BufferedWriter> tableWriterLookup = new HashMap<>();

    EventDealer(Path binlogFolder, String index) {
        this.binlogFolder = binlogFolder;
        this.index = index;
    }

    public void closeAll() throws IOException {
        for (ImmutablePair<String, String> key :
                tableAlterStateLookup.keySet()) {
            try (BufferedWriter countWriter = new BufferedWriter(getFileWriter(
                    binlogFolder.resolve(key.left)
                            .resolve(key.right)
                            .resolve(index)
                            .resolve("cnt.txt")
                            .toFile()
            ))) {
                countWriter.write(Integer.toString(tableAlterStateLookup.get(key) + 1));
            }
        }

        for (BufferedWriter writer :
                tableWriterLookup.values()) {
            writer.close();
        }
    }

    public void dealEvent(LogEvent event) throws IOException {
        if (event instanceof QueryLogEvent) {
            dealEventData((QueryLogEvent) event);
        }
        if (event instanceof RowsLogEvent) {
            dealEventData((RowsLogEvent) event);
        }
    }

    private FileWriter getFileWriter(File filetoWrite) throws IOException {
        boolean result = filetoWrite.getParentFile().mkdirs();
        return new FileWriter(filetoWrite);
    }

    public void dealEventData(RowsLogEvent event) throws IOException {
        AtomicLong sum = new AtomicLong(0);
        String dbName = event.getTable().getDbName();
        String tableName = event.getTable().getTableName().replace("`", "");
        String rowText = RowParser.parseRowsEvent(event, sum);
        tableWriterLookup.get(new ImmutablePair<>(dbName, tableName)).write(rowText);
    }


    public void dealEventData(QueryLogEvent event) throws IOException {
        if (event.getQuery().equals("BEGIN") || event.getQuery().equals("COMMIT")) return;
        SQLStatement statement_ = SQLParser.parse(event.getQuery());
        String dbName = event.getDbName();
//        System.out.println(event.getQuery());
        if (statement_ instanceof SQLCreateTableStatement) {
            SQLCreateTableStatement statement = (SQLCreateTableStatement) statement_;
            String tableName = statement.getTableName().replace("`", "");
            updateTableWriterAndDdlSql(dbName, tableName, statement);

        } else if (statement_ instanceof SQLAlterTableStatement) {
            SQLAlterTableStatement statement = (SQLAlterTableStatement) statement_;
            String tableName = statement.getTableName().replace("`", "");
            updateTableWriterAndDdlSql(dbName, tableName, statement);
        } else if (statement_ instanceof SQLDropDatabaseStatement) {
            dropTableWriterAndDdlSql(dbName);
        } else {
            if (!(statement_ instanceof SQLCreateDatabaseStatement))
                System.out.println("Not handled query! " + event.getQuery());
        }

    }

    private void updateTableWriterAndDdlSql(String dbName, String tableName, SQLStatement statement) throws IOException {
        if (!tableAlterStateLookup.containsKey(new ImmutablePair<>(dbName, tableName))) {
            tableAlterStateLookup.put(new ImmutablePair<>(dbName, tableName), 0);
        } else {
            tableWriterLookup.get(new ImmutablePair<>(dbName, tableName)).close();
            tableAlterStateLookup.put(new ImmutablePair<>(dbName, tableName),
                    tableAlterStateLookup.get(new ImmutablePair<>(dbName, tableName)) + 1);
        }
        tableWriterLookup.put(new ImmutablePair<>(dbName, tableName),
                new BufferedWriter(Objects.requireNonNull(getFileWriter(
                        binlogFolder.resolve(dbName)
                                .resolve(tableName)
                                .resolve(index)
                                .resolve(tableAlterStateLookup.get(new ImmutablePair<>(dbName, tableName)).toString() + ".dat")
                                .toFile()
                ))));
        try (BufferedWriter ddlWriter = new BufferedWriter(getFileWriter(
                binlogFolder.resolve(dbName)
                        .resolve(tableName)
                        .resolve(index)
                        .resolve(tableAlterStateLookup.get(new ImmutablePair<>(dbName, tableName)).toString() + ".ddlsql")
                        .toFile()
        ))) {
            ddlWriter.write(statement.toString());
        }
        DDLCompatWriter.writeDDLCompat(getFileWriter(
                binlogFolder.resolve(dbName)
                        .resolve(tableName)
                        .resolve(index)
                        .resolve(tableAlterStateLookup.get(new ImmutablePair<>(dbName, tableName)).toString() + ".ddl")
                        .toFile()
        ), statement);
    }

    private void dropTableWriterAndDdlSql(String dbName) throws IOException {
        for (HashMap.Entry<ImmutablePair<String, String>, BufferedWriter> entry : tableWriterLookup.entrySet())
            entry.getValue().close();
        tableWriterLookup.entrySet().removeIf(e -> e.getKey().left.equals(dbName));
        tableAlterStateLookup.entrySet().removeIf(e -> e.getKey().left.equals(dbName));
    }


}
