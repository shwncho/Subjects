public class Triangle extends ShapeBasics implements TriangleInterface{
    private int base;

    public Triangle(){
        super();
        base =0;
    }
    public Triangle(int theOffset, int theBase){
        super(theOffset);
        base = theBase;
    }

    public void set(int newBase){
        base = newBase;
    }

    public void drawHere(){
        drawTop();
        drawBase();
    }

    private void drawBase(){
        skipSpaces(getOffset());
        for(int count=0; count<base; count++){
            System.out.print('*');
        }
        System.out.print('*');
        System.out.println();
    }

    private void drawTop(){
        //startOfLine == number of spaces to the
        //first '*' on a line. Initially set to the
        //number of spaces before the topmost '*'.
        int startOfLine = getOffset()+base/2;
        skipSpaces(startOfLine);
        System.out.println('*');
        int lineCount = base / 2-1;

        //insideWidth == number of spaces between the
        //two '*'s on a line.
        int insideWidth=1;
        for(int count=0; count<lineCount; count++){

            //Down one line, so the first '*' is one more
            //space to the left.

            startOfLine--;
            skipSpaces(startOfLine);
            System.out.print('*');
            skipSpaces(insideWidth);
            System.out.println('*');

            //Down one line, so the inside is 2 spaces wider.
            insideWidth=insideWidth+2;
        }
    }



    private static void skipSpaces(int number){
        for(int count=0; count<number; count++){
            System.out.print(' ');
        }
    }
}
