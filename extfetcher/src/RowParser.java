import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.BitSet;

import com.taobao.tddl.dbsync.binlog.*;
import com.taobao.tddl.dbsync.binlog.event.*;

public class RowParser {
    protected static Charset charset = StandardCharsets.UTF_8;

    public static void parseRowsEvent(RowsLogEvent event, StringBuilder builder) {
        try {
            RowsLogBuffer buffer = event.getRowsBuf(charset.name());
            BitSet columns = event.getColumns();
            BitSet changeColumns = event.getChangeColumns();
            while (buffer.nextOneRow(columns)) {
                int type = event.getHeader().getType();
                if (LogEvent.WRITE_ROWS_EVENT_V1 == type || LogEvent.WRITE_ROWS_EVENT == type) {
                    parseOneRow(event, buffer, columns, true, builder);
                } else if (LogEvent.DELETE_ROWS_EVENT_V1 == type || LogEvent.DELETE_ROWS_EVENT == type) {
                    parseOneRow(event, buffer, columns, false, builder);
                } else {
                    parseOneRow(event, buffer, columns, false, builder);
                    if (!buffer.nextOneRow(changeColumns, true)) {
                        break;
                    }
                    parseOneRow(event, buffer, changeColumns, true, builder);
                }
            }
        } catch (Exception e) {
            throw new RuntimeException("parse row data failed.", e);
        }
    }

    public static void parseOneRow(RowsLogEvent event, RowsLogBuffer buffer, BitSet cols, boolean isAfter, StringBuilder builder) {
        TableMapLogEvent map = event.getTable();
        if (map == null) {
            throw new RuntimeException("not found TableMap with tid=" + event.getTableId());
        }
        final int columnCnt = map.getColumnCnt();
        final TableMapLogEvent.ColumnInfo[] columnInfo = map.getColumnInfo();
        for (int i = 0; i < columnCnt; i++) {
            if (!cols.get(i)) {
                continue;
            }
            TableMapLogEvent.ColumnInfo info = columnInfo[i];
            buffer.nextValue(null, i, info.type, info.meta);
            if (!buffer.isNull()) {
                if (isAfter) {
                    builder.append("\"");
                    if (buffer.getValue() instanceof String)
                        EscapeConvert.convert((String) buffer.getValue(),builder);
                    else builder.append(buffer.getValue().toString());
                    builder.append("\",");
                }
            } else{
                if (isAfter) {
                    builder.append("\"_NUll_&\",");
                }
            }
        }
        if (isAfter) {
            builder.deleteCharAt(builder.length() - 1);
            builder.append("\n");
        }

    }
}