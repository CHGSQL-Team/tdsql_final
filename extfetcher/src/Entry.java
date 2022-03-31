import java.io.IOException;

public class Entry {
    public static void canalSplit(String arg) throws InterruptedException {
        String[] args = arg.split(" ");
        main(args);
    }

    public static void main(String[] args) throws InterruptedException {
        Portal pt1 = new Portal(args[0], args[1], args[2], args[3]);
        Portal pt2 = new Portal(args[4], args[5], args[6], args[7]);
        pt1.start();
        pt2.start();
        pt1.join();
        pt2.join();
    }
}

class Portal extends Thread {
    String eventBinPath;
    String eventLenPath;
    String binlogPath_;
    String sourceIndex;

    public Portal(String eventBinPath, String eventLenPath, String binlogPath_, String sourceIndex) {
        this.eventBinPath = eventBinPath;
        this.eventLenPath = eventLenPath;
        this.binlogPath_ = binlogPath_;
        this.sourceIndex = sourceIndex;
    }

    public void run() {
        CanalSplitter splitter = new CanalSplitter();
        try {
            splitter.doSplit(eventBinPath, eventLenPath, binlogPath_, sourceIndex);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
