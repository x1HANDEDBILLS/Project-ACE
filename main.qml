import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 480
    title: "Project ACE - Cockpit"
    color: "#0a0a0a" // Deep black

    // Background Glow
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1a1a1a" }
            GradientStop { position: 1.0; color: "#000000" }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 40
        spacing: 30

        // Header Section
        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "PROJECT ACE"
                color: "#00ffcc"
                font.pixelSize: 32
                font.bold: true
                font.letterSpacing: 2
            }
            Item { Layout.fillWidth: true } // Spacer
            Text {
                text: "CORE 3: 1000Hz ACTIVE"
                color: "#555"
                font.pixelSize: 14
            }
        }

        // Status Indicator
        Rectangle {
            Layout.fillWidth: true
            height: 120
            color: "#111"
            radius: 10
            border.color: "#333"

            Column {
                anchors.centerIn: parent
                spacing: 10
                
                Text {
                    text: "NOMAD LINK QUALITY"
                    color: "#aaa"
                    anchors.horizontalCenter: parent
                }

                // This bar "listens" to the C++ property
                ProgressBar {
                    id: linkBar
                    width: 400
                    value: aceEngine.signalStrength // <-- THE BRIDGE
                    
                    background: Rectangle { height: 10; radius: 5; color: "#222" }
                    contentItem: Item {
                        Rectangle {
                            width: linkBar.visualPosition * parent.width
                            height: 10; radius: 5
                            color: linkBar.value > 0.5 ? "#00ff00" : "#ff3300"
                        }
                    }
                }
            }
        }

        // Footer Information
        Text {
            text: "Link Status: Connected (USB xHCI Tuned)"
            color: "#666"
            font.pixelSize: 12
        }
    }
}