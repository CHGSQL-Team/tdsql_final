import com.github.shyiko.mysql.binlog.*;
import com.github.shyiko.mysql.binlog.event.*;
import com.github.shyiko.mysql.binlog.event.deserialization.EventDeserializer;

import java.io.File;
import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;

import static java.lang.Integer.parseInt;

public class FileMain {

    public static void main(String[] args) throws IOException {
        File binlogFile = new File(args[0]);
        Path outFolder = Paths.get(args[1]);
        EventDeserializer eventDeserializer = new EventDeserializer();
        eventDeserializer.setCompatibilityMode(
                EventDeserializer.CompatibilityMode.DATE_AND_TIME_AS_LONG,
                EventDeserializer.CompatibilityMode.CHAR_AND_BINARY_AS_BYTE_ARRAY
        );
        EventDealer dealer = new EventDealer(outFolder);
        try (BinaryLogFileReader reader = new BinaryLogFileReader(binlogFile, eventDeserializer)) {
            for (Event event; (event = reader.readEvent()) != null; ) {
                dealer.dealEvent(event);
            }
        }
    }
}
