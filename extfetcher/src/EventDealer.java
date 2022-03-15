import com.github.shyiko.mysql.binlog.event.*;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.nio.file.Path;
import java.util.HashMap;

public class EventDealer {
    Path outFolder;
    Path ddlFile;
    HashMap<Long, String> tableIdDataBaseLookup = new HashMap<>();
    HashMap<Long, String> tableIdTableLookup = new HashMap<>();
    BufferedWriter ddlSqlWriter;

    EventDealer(Path outFolder) {
        this.outFolder = outFolder.resolve("extfetcher");
        if (outFolder.toFile().mkdirs()) {
            System.out.println("Mkdir Success.");
        }
        this.ddlFile = outFolder.resolve("ddl.sql");
    }

    public void dealEvent(Event event) {
//        if (event.getData() instanceof GtidEventData) {
//            dealEventData((GtidEventData) event.getData());
//        }
//        if (event.getData() instanceof PreviousGtidSetEventData) {
//            dealEventData((PreviousGtidSetEventData) event.getData());
//        }
//        if (event.getData() instanceof TableMapEventData) {
//            dealEventData((TableMapEventData) event.getData());
//        }
//        if (event.getData() instanceof WriteRowsEventData) {
//            dealEventData((WriteRowsEventData) event.getData());
//        }
        if (event.getData() instanceof QueryEventData) {
            dealEventData((QueryEventData) event.getData());
        }


    }

    private void dealEventData(GtidEventData eventData) {

    }

    private void dealEventData(PreviousGtidSetEventData eventData) {

    }

    private void dealEventData(TableMapEventData eventData) {
        tableIdDataBaseLookup.put(eventData.getTableId(), eventData.getDatabase());
        tableIdTableLookup.put(eventData.getTableId(), eventData.getTable());
    }

    private void dealEventData(WriteRowsEventData eventData) {
        System.out.println("Recv WriteRows to " + tableIdDataBaseLookup.get(eventData.getTableId()) + "/" + tableIdTableLookup.get(eventData.getTableId()));
    }

    private void dealEventData(QueryEventData eventData) {
        String sql = eventData.getSql();
        System.out.println(sql);
    }

}
