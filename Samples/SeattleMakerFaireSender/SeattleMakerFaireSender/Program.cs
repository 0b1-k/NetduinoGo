using System;
using System.IO;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoGo;

using Nwazet.Go.SD;
using Nwazet.Go.Imaging;
using Nwazet.Go.Display.TouchScreen;
using Nwazet.Go.Helpers;

namespace SeattleMakerFaireSender {
    public class Program {
        public static GoBus.GoSocket DisplaySocket = GoSockets.Socket4;
        public static GoBus.GoSocket SDSocket = GoSockets.Socket6;

        public static void Main() {
            Debug.EnableGCMessages(true);
            Debug.Print("Available RAM: " + Debug.GC(true).ToString() + " bytes.");
            Debug.EnableGCMessages(false);

            var canvas = new VirtualCanvas(null, null);
            canvas.Initialize(DisplaySocket);
            CalibrateTouchscreen(canvas);

            var mainMenu = new MainMenu(canvas);
            mainMenu.Render(canvas, SDSocket);
        }
        public static void CalibrateTouchscreen(VirtualCanvas canvas) {
            var sd = new SDCardReader();
            try {
                sd.Initialize(SDSocket);
                var calibrationDataFilename = @"SD\TouchscreenCalibration.bin";
                if (File.Exists(calibrationDataFilename)) {
                    using (var calibrationDataFile = new FileStream(calibrationDataFilename, FileMode.Open)) {
                        var context = new BasicTypeDeSerializerContext(calibrationDataFile);
                        var matrix = new CalibrationMatrix();
                        matrix.Get(context);
                        canvas.SetTouchscreenCalibrationMatrix(matrix);
                    }
                } else {
                    using (var calibrationDataFile = new FileStream(calibrationDataFilename, FileMode.Create)) {
                        var matrix = canvas.GetTouchscreenCalibrationMatrix();
                        var context = new BasicTypeSerializerContext(calibrationDataFile);
                        matrix.Put(context);
                    }
                }
            } catch (Exception e) {
                Debug.Print(e.Message);
                Debug.Print("SD Card or file I/O error: manual calibration required.");
                canvas.TouchscreenCalibration();
            }
            sd.Dispose();
        }
    }
}
