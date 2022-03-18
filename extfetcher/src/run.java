import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.Arrays;

public class run {



    public static String GtidResolverCall(String arg) throws IOException {
        return GtidResolver.main(arg);
    }

    public static String TestCall(String arg){
        return "123";
    }
}
