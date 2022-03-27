public class EscapeConvert {
    public static void convert(String toConvert, StringBuilder builder){
        int length = toConvert.length();
        for(int i=0;i<length;i++){
            char curChar = toConvert.charAt(i);
            if(curChar=='\"') builder.append("\\");
            builder.append(curChar);
        }
    }
}
