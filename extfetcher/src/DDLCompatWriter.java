import com.alibaba.druid.sql.ast.SQLIndexDefinition;
import com.alibaba.druid.sql.ast.SQLStatement;
import com.alibaba.druid.sql.ast.statement.*;
import com.alibaba.druid.sql.dialect.mysql.ast.MySqlKey;
import com.alibaba.druid.sql.dialect.mysql.ast.MySqlPrimaryKey;
import com.alibaba.druid.sql.dialect.mysql.ast.MySqlUnique;
import com.alibaba.druid.sql.dialect.mysql.ast.statement.MySqlAlterTableAlterColumn;
import com.alibaba.druid.sql.dialect.mysql.ast.statement.MySqlAlterTableChangeColumn;
import com.alibaba.druid.sql.dialect.mysql.ast.statement.MySqlCreateTableStatement;
import com.alibaba.druid.sql.dialect.mysql.ast.statement.MysqlAlterTableAlterCheck;
import org.apache.ibatis.jdbc.SQL;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

public class DDLCompatWriter {
    public static void writeDDLCompat(FileWriter ddlFileWriter, SQLStatement statement) throws IOException {
        BufferedWriter writer = new BufferedWriter(ddlFileWriter);
        if (statement instanceof MySqlCreateTableStatement) {
            dealCreateTable(writer, (MySqlCreateTableStatement) statement);
        } else if (statement instanceof SQLAlterTableStatement) {
            dealAlterTable(writer, (SQLAlterTableStatement) statement);
        } else System.out.println("Not handled DDL!");
        writer.close();
    }

    public static void dealCreateTable(BufferedWriter writer, MySqlCreateTableStatement statement) throws IOException {
        writer.write("CREATE");
        writer.newLine();
        writer.write(statement.getTableName().toString().replace("`", ""));
        writer.newLine();
        writer.write(Integer.toString(statement.getColumnDefinitions().size()));
        writer.newLine();
        for (SQLColumnDefinition colDef :
                statement.getColumnDefinitions()) {
            writer.write(colDef.getColumnName().replace("`", ""));
            writer.newLine();
            if (checkIfNotNull(colDef.getConstraints())) writer.write("1");
            else writer.write("0");
            writer.newLine();
            if (colDef.getDefaultExpr() == null) {
                writer.write("0");
            } else {
                writer.write("1");
                writer.newLine();
                writer.write(colDef.getDefaultExpr().toString().replace("'", ""));
            }
            writer.newLine();
        }

        int realKeysCount = 0;
        for (MySqlKey key :
                statement.getMysqlKeys()) {
            if (key.getIndexDefinition().getType() != null) realKeysCount++;
        }
        writer.write(Integer.toString(realKeysCount));
        writer.newLine();

        for (MySqlKey key :
                statement.getMysqlKeys()) {
            if (key.getIndexDefinition().getType() == null) continue;
            if (key.getName() == null) {
                writer.write("0");
            } else {
                writer.write("1");
                writer.newLine();
                writer.write(key.getName().getSimpleName());
            }
            writer.newLine();
            if (key instanceof MySqlPrimaryKey) writer.write("1");
            else writer.write("0");
            writer.newLine();
            writer.write(Integer.toString(key.getColumns().size()));
            writer.newLine();
            for (SQLSelectOrderByItem item :
                    key.getColumns()) {
                writer.write(item.toString().replace("`", ""));
                writer.newLine();
            }
        }
    }

    public static <SQLALterTableAddIndex> void dealAlterTable(BufferedWriter writer, SQLAlterTableStatement statement) throws IOException {
        writer.write("ALTER");
        writer.newLine();
        SQLAlterTableItem item = statement.getItems().get(0);

        if (item instanceof SQLAlterTableAddColumn) {
            writer.write("ADDCOL");
            writer.newLine();
            SQLAlterTableAddColumn addColItem = (SQLAlterTableAddColumn) item;
            writer.write(addColItem.getColumns().get(0).getColumnName());
            writer.newLine();
            if (!addColItem.getColumns().get(0).getConstraints().isEmpty()
                    && addColItem.getColumns().get(0).getConstraints().get(0) instanceof SQLNotNullConstraint)
                writer.write("1");
            else writer.write("0");
            writer.newLine();
            if (addColItem.getColumns().get(0).getDefaultExpr() != null) {
                writer.write("1");
                writer.newLine();
                writer.write(addColItem.getColumns().get(0).getDefaultExpr().toString().replace("'", ""));
            } else {
                writer.write("0");
            }
            writer.newLine();
            if (addColItem.getAfterColumn() != null) {
                writer.write("1");
                writer.newLine();
                writer.write(addColItem.getAfterColumn().getSimpleName().replace("`", ""));
                writer.newLine();
            } else writer.write("0");
            writer.newLine();
        } else if (item instanceof SQLAlterTableDropColumnItem) {
            SQLAlterTableDropColumnItem dropColItem = (SQLAlterTableDropColumnItem) item;
            writer.write("DROPCOL");
            writer.newLine();
            writer.write(dropColItem.getColumns().get(0).getSimpleName());
            writer.newLine();
        } else if (item instanceof SQLAlterTableAddIndex) {
            writer.write("NOTHING");
            writer.newLine();
        } else if (item instanceof SQLAlterTableDropIndex) {
            SQLAlterTableDropIndex dropIndex = (SQLAlterTableDropIndex) item;
            writer.write("DROPINDEX");
            writer.newLine();
            writer.write("0");
            writer.newLine();
            writer.write(String.valueOf(dropIndex.getIndexName()));
            writer.newLine();
        } else if (item instanceof SQLAlterTableDropPrimaryKey) {
            writer.write("DROPINDEX");
            writer.newLine();
            writer.write("1");
            writer.newLine();
        } else if (item instanceof SQLAlterTableAddConstraint) {
            SQLAlterTableAddConstraint addConst = (SQLAlterTableAddConstraint) item;
            writer.write("ADDINDEX");
            writer.newLine();
            List<SQLSelectOrderByItem> cols = null;
            if (addConst.getConstraint() instanceof MySqlPrimaryKey) {
                MySqlPrimaryKey constraint = (MySqlPrimaryKey) addConst.getConstraint();
                writer.write("1");
                writer.newLine();
                cols = constraint.getColumns();
            } else if (addConst.getConstraint() instanceof MySqlUnique) {
                MySqlUnique constraint = (MySqlUnique) addConst.getConstraint();
                writer.write("0");
                writer.newLine();
                writer.write(constraint.getName().getSimpleName());
                writer.newLine();
                cols = constraint.getColumns();
            } else System.out.println("Unknown constraint!");

            assert cols != null;
            writer.write(Integer.toString(cols.size()));
            writer.newLine();
            for (SQLSelectOrderByItem col : cols) {
                writer.write(col.toString());
                writer.newLine();
            }
        } else if (item instanceof MySqlAlterTableChangeColumn) {
            writer.write("CHANGECOL");
            writer.newLine();
            MySqlAlterTableChangeColumn chCol = (MySqlAlterTableChangeColumn) item;
            SQLColumnDefinition def = chCol.getNewColumnDefinition();
            writer.write(def.getColumnName());
            writer.newLine();
            if (def.containsNotNullConstaint()) writer.write("1");
            else writer.write("0");
            writer.newLine();
            if (def.getDefaultExpr() != null) {
                writer.write("1");
                writer.newLine();
                writer.write(def.getDefaultExpr().toString().replace("'", ""));
            } else writer.write("0");
            writer.newLine();
            writer.write(chCol.getColumnName().getSimpleName());
            writer.newLine();
        } else {
            System.out.println("Not handled Alter! " + statement + " which should be " + item.getClass());
        }

    }

    public static boolean checkIfNotNull(List<SQLColumnConstraint> contraintList) {
        for (SQLColumnConstraint cons :
                contraintList) {
            if (cons instanceof SQLNotNullConstraint) return true;
        }
        return false;
    }
}
