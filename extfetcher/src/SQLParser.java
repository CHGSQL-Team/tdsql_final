import com.alibaba.druid.sql.ast.SQLStatement;
import com.alibaba.druid.sql.dialect.mysql.parser.MySqlStatementParser;

public class SQLParser {
    public static SQLStatement parse(String sql) {
        MySqlStatementParser parser = new MySqlStatementParser(sql);
        return parser.parseStatement();
    }
}