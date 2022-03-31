import com.taobao.tddl.dbsync.binlog.*;

import java.io.*;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;

public class CanalSplitter {
    EventDealer eventDealer;


    public static void main(String[] args) throws IOException {
        CanalSplitter splitter = new CanalSplitter();
        splitter.doSplit(args[0], args[1], args[2], args[3]);
    }

    public static void portal(String arg) throws IOException {
        String[] args = arg.split(" ");
        CanalSplitter splitter = new CanalSplitter();
        splitter.doSplit(args[0], args[1], args[2], args[3]);
    }

    public void doSplit(String eventBinPath, String eventLenPath, String binlogPath_, String sourceIndex) throws IOException {

        File eventBinFile = new File(eventBinPath);
        File eventLenFile = new File(eventLenPath);
        Path binlogPath = Paths.get(binlogPath_);
        Scanner lenReader = new Scanner(eventLenFile);
        eventDealer = new EventDealer(binlogPath, sourceIndex);
        BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(eventBinFile));
        LogDecoder decoder = new LogDecoder(LogEvent.QUERY_EVENT, LogEvent.TABLE_MAP_EVENT);
        LogContext context = new LogContext();
        context.setLogPosition(new LogPosition("", 4));
        int position = 0;

        // Pre deal event
        while (lenReader.hasNextInt()) {
            int eventLength = lenReader.nextInt();
            byte[] eventByte = new byte[eventLength];
            int readBytes = inputStream.read(eventByte, 0, eventLength);
            LogBuffer buf = new LogBuffer(eventByte, 0, eventLength);
            LogEvent event = decoder.decode(buf, context);
            eventDealer.proofEvent(event, position++);
        }

        lenReader = new Scanner(eventLenFile);
        inputStream = new BufferedInputStream(new FileInputStream(eventBinFile));
        decoder = new LogDecoder(LogEvent.UNKNOWN_EVENT, LogEvent.ENUM_END_EVENT);
        context = new LogContext();
        context.setLogPosition(new LogPosition("", 4));
        position = 0;
        //Deal event actually
        while (lenReader.hasNextInt()) {
            int eventLength = lenReader.nextInt();
            byte[] eventByte = new byte[eventLength];
            int readBytes = inputStream.read(eventByte, 0, eventLength);
            LogBuffer buf = new LogBuffer(eventByte, 0, eventLength);
            LogEvent event = decoder.decode(buf, context);
            eventDealer.dealEvent(event, position++);
        }
        eventDealer.closeAll();

    }
}
