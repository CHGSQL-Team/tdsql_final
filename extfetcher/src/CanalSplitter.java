import com.alibaba.otter.canal.instance.manager.model.Canal;
import com.taobao.tddl.dbsync.binlog.*;

import java.io.*;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;

public class CanalSplitter {

    public void doSplit(String eventBinPath, String eventLenPath, String binlogPath_, String sourceIndex) throws IOException, InterruptedException {
        LogDecoder decoder = new LogDecoder(LogEvent.UNKNOWN_EVENT, LogEvent.ENUM_END_EVENT);
        LogContext context = new LogContext();
        context.setLogPosition(new LogPosition("", 4));
        File eventBinFile = new File(eventBinPath);
        File eventLenFile = new File(eventLenPath);
        Path binlogPath = Paths.get(binlogPath_);
        Scanner lenReader = new Scanner(eventLenFile);
        EventDealer eventDealer = new EventDealer(binlogPath, sourceIndex);
        BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(eventBinFile));

        while (lenReader.hasNextInt()) {
            int eventLength = lenReader.nextInt();
            byte[] eventByte = new byte[eventLength];
            int readBytes = inputStream.read(eventByte, 0, eventLength);
            LogBuffer buf = new LogBuffer(eventByte, 0, eventLength);
            LogEvent event = decoder.decode(buf, context);
            eventDealer.dealEvent(event);

        }
        eventDealer.waitIt();
        eventDealer.closeAll();

    }
}
