/* $Id$ */
import java.awt.*;
import java.awt.image.*;
import java.awt.geom.*;

public class Waveform_2 extends Waveform
{
    protected void DrawImage(Graphics g, Object img, Dimension dim)
    {
       if(!(img instanceof RenderedImage))
       {
           super.DrawImage(g, img, dim);
       }
       else
       {
           ((Graphics2D)g).clearRect(0, 0, dim.width, dim.height);
           ((Graphics2D)g).drawRenderedImage((RenderedImage)img, new AffineTransform(1f,0f,0f,1f,0F,0F));
       }
    }
}
