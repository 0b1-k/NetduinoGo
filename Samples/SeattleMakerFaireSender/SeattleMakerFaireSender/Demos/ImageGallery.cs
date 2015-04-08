using System;
using Microsoft.SPOT;
using Nwazet.Go.Imaging;
using Nwazet.Go.SD;
namespace SeattleMakerFaireSender.Demos {
    public class ImageGallery {
        public void Render(VirtualCanvas canvas, GoBus.GoSocket SDSocket) {
            var sd = new SDCardReader();
            try {
                sd.Initialize(SDSocket);
                canvas.SetOrientation(Orientation.Portrait);
                DisplayBmpPicture(canvas, @"Nwazet\03.bmp");
                DisplayBmpPicture(canvas, @"Nwazet\05.bmp");
                DisplayBmpPicture(canvas, @"Nwazet\09.bmp");
                canvas.SetOrientation(Orientation.Landscape);
                DisplayBmpPicture(canvas, @"Nwazet\00.bmp");
                DisplayBmpPicture(canvas, @"Nwazet\01.bmp");
                DisplayBmpPicture(canvas, @"Nwazet\02.bmp");
                DisplayBmpPicture(canvas, @"Nwazet\04.bmp");
                DisplayBmpPicture(canvas, @"Nwazet\06.bmp");
                DisplayBmpPicture(canvas, @"Nwazet\07.bmp");
                DisplayBmpPicture(canvas, @"Nwazet\08.bmp");
            } catch (Exception e) {
                Debug.Print(e.Message);
                Debug.Print("You need an SD card loaded with the demo photos to run this part of the demo.");
            }
            sd.Dispose();
        }
        private void DisplayBmpPicture(VirtualCanvas canvas, string pictureName) {
            canvas.DrawBitmapImage(0, 0, @"SD\" + pictureName);
            canvas.TouchscreenWaitForEvent();
        }
    }
}
