import QtQuick 2.15
import QtQuick.Window 2.15
import AceModule 1.0   // ← This matches your CMake URI "AceModule"

Window {
    width: 900
    height: 700
    visible: true
    title: "ACE LIVE CONTROLLER DASHBOARD"

    Logic {
        id: logic
    }

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "ACE LIVE INPUT"
            font.pixelSize: 32
            font.bold: true
        }

        Text { text: "Connected: " + logic.connected; font.pixelSize: 22 }

        Text { text: "Left Stick  → X: " + logic.axes[0] + "   Y: " + logic.axes[1]; font.pixelSize: 20 }
        Text { text: "Right Stick → X: " + logic.axes[2] + "   Y: " + logic.axes[3]; font.pixelSize: 20 }

        Text { text: "Buttons: " + logic.buttons; font.pixelSize: 20 }

        Text { text: "Gyro: " + logic.gyro; font.pixelSize: 20 }

        Text { text: "Touch → X: " + logic.touch.x.toFixed(2) + "  Y: " + logic.touch.y.toFixed(2) + "  Down: " + logic.touch.down; font.pixelSize: 20 }
    }
}