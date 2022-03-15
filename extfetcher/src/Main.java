import com.github.shyiko.mysql.binlog.*;
import com.github.shyiko.mysql.binlog.event.*;
import com.github.shyiko.mysql.binlog.event.deserialization.EventDeserializer;

import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;

import static java.lang.Integer.parseInt;

public class Main {

    public static void main(String[] args) throws IOException {
        BinaryLogClient client = new BinaryLogClient(args[0], parseInt(args[1]), args[2], args[3]);
        String startGtid = args[4];
        Path outFolder = Paths.get(args[5]);
        EventDeserializer eventDeserializer = new EventDeserializer();
        eventDeserializer.setCompatibilityMode(
                EventDeserializer.CompatibilityMode.DATE_AND_TIME_AS_LONG,
                EventDeserializer.CompatibilityMode.CHAR_AND_BINARY_AS_BYTE_ARRAY
        );
        client.setEventDeserializer(eventDeserializer);
//        EventDealer eventDealer = new EventDealer(startGtid, outFolder);
        client.registerEventListener(new BinaryLogClient.EventListener() {
            @Override
            public void onEvent(Event event) {
//                eventDealer.dealEvent(event);
                System.out.println(event);
            }
        });
        client.setBlocking(false);
        client.setBinlogFilename("");
        client.setBinlogPosition(4);
        client.connect();
    }
}
