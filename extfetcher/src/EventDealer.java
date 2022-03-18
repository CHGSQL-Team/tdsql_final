//import com.alibaba.otter.canal.protocol.CanalEntry;

import java.io.*;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicLong;

import com.alibaba.druid.sql.ast.SQLStatement;
import com.alibaba.druid.sql.ast.statement.SQLAlterTableStatement;
import com.alibaba.druid.sql.ast.statement.SQLCreateTableStatement;
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
        for (BufferedWriter writer:
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
        String tableName = event.getTable().getTableName().replace("`","");
        String rowText = RowParser.parseRowsEvent(event, sum);
        tableWriterLookup.get(new ImmutablePair<>(dbName, tableName)).write(rowText);
    }


    public void dealEventData(QueryLogEvent event) throws IOException {
        if (event.getQuery().equals("BEGIN")) return;
        SQLStatement statement_ = SQLParser.parse(event.getQuery());
        String dbName = event.getDbName();
        if (statement_ instanceof SQLCreateTableStatement) {
            SQLCreateTableStatement statement = (SQLCreateTableStatement) statement_;
            String tableName = statement.getTableName().replace("`", "");

            updateTableWriterAndDdlSql(dbName, tableName, statement.toString());
        }

        if (statement_ instanceof SQLAlterTableStatement) {
            SQLAlterTableStatement statement = (SQLAlterTableStatement) statement_;
            String tableName = statement.getTableName().replace("`", "");
            updateTableWriterAndDdlSql(dbName, tableName, statement.toString());
        }
    }

    private void updateTableWriterAndDdlSql(String dbName, String tableName, String s) throws IOException {
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
            ddlWriter.write(s);
        }
    }

//    private String dealEventData(GtidEventData eventData) {
//        return "";
//    }
//
//    private String dealEventData(PreviousGtidSetEventData eventData) {
//        return "";
//    }
//
//    private String dealEventData(TableMapEventData eventData) {
////        idTableLookup.put(eventData.getTableId(), new ImmutablePair<>(eventData.getDatabase(), eventData.getTable()));
//        return "";
//    }
//
//    private String dealEventData(WriteRowsEventData eventData) {
//        return eventData.toString() + "\n";
//
//    }
//
//    private String dealEventData(QueryEventData eventData) throws IOException {
//        String sql = eventData.getSql();
//        String dataBase = eventData.getDatabase();
//        if (sql.contains("alter table") || sql.contains("ALTER TABLE")) {
//            return sql;
//
////            String[] sql_splited = sql.split(" ");
////            String table = sql_splited[5].substring(1, sql_splited[5].length() - 1);
////            tableAlterStateLookup.put(new ImmutablePair<>(dataBase, table), 0);
////            File outFile = outFolder.resolve(dataBase).resolve(table).resolve(index).resolve("0.txt").toFile();
////            if (!outFile.getParentFile().mkdirs()) {
////                System.out.println("Failed making dirs");
////            }
////            tableWriterLookup.put(new ImmutablePair<>(dataBase, table), new BufferedWriter(new FileWriter(outFile)));
//        }
//        return "";
//    }

}
