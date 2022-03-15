import java.io.IOException;

import com.github.shyiko.mysql.binlog.*;

public class GtidResolver {
    public static void main(String[] args) throws IOException {
        String gtidSetString = args[0];
        GtidSet gtidSet = new GtidSet(gtidSetString);
        byte[] gtidCommand = gtidSet.getCommandPackage();
        StringBuilder sb = new StringBuilder();
        for (int i = 4; i < gtidCommand.length; i++) {
            sb.append(String.format("%02X ", gtidCommand[i]));
        }
        System.out.println(sb.toString());
    }
}
