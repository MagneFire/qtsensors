/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

//Import the declarative plugins
import QtQuick 2.0

Item {
    id: textInputFrame
    property alias text: textInput.text
    resources: [
        Component{
            id: cursorA
            Rectangle {
                id: cursor_rect
                width: 2
                height: 20
                color: "#1c94ff"
                visible: textInput.cursorVisible

                PropertyAnimation on opacity  {
                    easing.type: Easing.OutSine
                    loops: Animation.Infinite
                    from: 0
                    to: 1.0
                    duration: 750
                }
            }
        }
    ]

    Image {
        id: backgroundImage
        anchors.fill: parent
        source: (textInputFrame.enabled ? "images/textfield_background_normal.png" : "images/textfield_background_disabled.png")
    }

    TextInput {
        id: textInput
        anchors.fill: parent
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        activeFocusOnPress: false

        cursorDelegate: cursorA

        onEnabledChanged: {
             textInput.focus = false;
        }

        MouseArea {
             anchors.fill: parent

             onClicked: {
                if (!textInput.activeFocus) {
                    textInput.forceActiveFocus()
                    textInput.openSoftwareInputPanel();
                } else {
                    textInput.focus = false;
                }
            }

            onPressAndHold: textInput.closeSoftwareInputPanel();
        }
    }
}