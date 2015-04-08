namespace Nwazet.Go.Display.TouchScreen {
    public class ButtonWidget : Widget {
        public ButtonWidget(int x0, int y0, int x1, int y1) {
            DefineClickableArea(new ClickableArea(x0, y0, x1, y1));
        }
    }
}
