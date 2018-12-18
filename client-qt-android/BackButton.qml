import QtQuick 2.12
import QtQuick.Window 2.12

Image {
    id: backButton
    signal clicked()
    width: 50
    z: 1
    anchors.left: parent.left
    anchors.top: parent.top
    source: "src_img/back.png"
    fillMode: Image.PreserveAspectFit

    MouseArea {
        id: handler
        anchors.fill: parent
        onClicked: backButton.clicked()
    }
}
