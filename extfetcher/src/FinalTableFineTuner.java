import com.alibaba.druid.sql.dialect.mysql.ast.MySqlKey;
import com.alibaba.druid.sql.dialect.mysql.ast.statement.MySqlCreateTableStatement;

import java.util.ArrayList;
import java.util.List;

public class FinalTableFineTuner {
    public static void fineTune(MySqlCreateTableStatement createTableStmt) {
        List<MySqlKey> uniqKey = new ArrayList<>();
        List<MySqlKey> normKey = new ArrayList<>();
        for (MySqlKey key :
                createTableStmt.getMysqlKeys()) {
            if (key.getIndexDefinition().getType() == null) {
                normKey.add(key);
            } else {
                uniqKey.add(key);
            }
        }
        if (!uniqKey.isEmpty()) {
            for (MySqlKey key :
                    normKey) {
                createTableStmt.getTableElementList().removeIf(e -> e == key);
            }
        }
    }
}
