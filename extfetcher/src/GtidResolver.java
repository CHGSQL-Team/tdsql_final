import java.io.IOException;

public class GtidResolver {
    public static String main(String gtidSetString) throws IOException {
        GtidSet gtidSet = new GtidSet(gtidSetString);
        byte[] gtidCommand = gtidSet.getCommandPackage();
        StringBuilder sb = new StringBuilder();
        for (int i = 4; i < gtidCommand.length; i++) {
            sb.append(String.format("%02X ", gtidCommand[i]));
        }
        return sb.toString();
    }
}
