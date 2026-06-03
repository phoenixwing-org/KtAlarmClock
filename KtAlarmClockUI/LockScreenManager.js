/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @file        LockScreenManager.js
 * @brief       Manage secondary lock-screen overlays per display
 */
.pragma library

var _overlays = []

function clearOverlays() {
    for (var i = _overlays.length - 1; i >= 0; --i) {
        if (_overlays[i])
            _overlays[i].destroy()
    }
    _overlays = []
}

function hideAll() {
    clearOverlays()
}

function findOverlay(screenId) {
    for (var i = 0; i < _overlays.length; ++i) {
        if (_overlays[i] && _overlays[i].screenId === screenId)
            return _overlays[i]
    }
    return null
}

function expectedScreenIds(primaryScreenId) {
    var screens = Qt.application.screens
    var ids = []

    if (KtAlarmTheme.debug !== 0) {
        if (screens.length > 0)
            ids.push(primaryScreenId)
        return ids
    }

    for (var i = 0; i < screens.length; ++i) {
        if (i !== primaryScreenId)
            ids.push(i)
    }
    return ids
}

function createOverlay(over0, screenId, onUnlock) {
    var component = Qt.createComponent("qrc:/MyOver1.qml")
    if (component.status !== Component.Ready) {
        console.log("LockScreenManager: failed to load MyOver1.qml", component.errorString())
        return null
    }

    var overlay = component.createObject(null, {
        screenId: screenId,
        fullScreen: over0.fullScreen,
        canClose: over0.canClose
    })
    if (!overlay) {
        console.log("LockScreenManager: failed to create overlay for screen", screenId)
        return null
    }

    overlay.isForced = Qt.binding(function() { return over0.isForced })
    overlay.counterForce = Qt.binding(function() { return over0.counterForce })
    overlay.sendClose.connect(onUnlock)

    if (!overlay.applyScreenGeometry()) {
        overlay.destroy()
        return null
    }

    if (KtAlarmTheme.debug !== 0) {
        overlay.x = over0.x + over0.width
        overlay.color = "grey"
        overlay.show()
    } else if (!overlay.showOver()) {
        overlay.destroy()
        return null
    }

    _overlays.push(overlay)
    return overlay
}

function reconcileScreens(over0, onUnlock) {
    if (!over0)
        return

    var primaryScreenId = over0.screenId
    var needed = expectedScreenIds(primaryScreenId)

    for (var i = _overlays.length - 1; i >= 0; --i) {
        var existing = _overlays[i]
        if (!existing || needed.indexOf(existing.screenId) < 0) {
            if (existing)
                existing.destroy()
            _overlays.splice(i, 1)
        }
    }

    for (var n = 0; n < needed.length; ++n) {
        var screenId = needed[n]
        var overlay = findOverlay(screenId)
        if (overlay) {
            overlay.fullScreen = over0.fullScreen
            if (KtAlarmTheme.debug !== 0) {
                overlay.x = over0.x + over0.width
                overlay.show()
            } else {
                overlay.applyScreenGeometry()
                if (overlay.visible)
                    overlay.raiseToFront()
            }
        } else {
            createOverlay(over0, screenId, onUnlock)
        }
    }
}

function raiseAll(over0) {
    if (over0 && over0.visible)
        over0.raiseToFront()

    for (var i = 0; i < _overlays.length; ++i) {
        if (_overlays[i] && _overlays[i].visible)
            _overlays[i].raiseToFront()
    }
}

function applyAllGeometry(over0) {
    if (over0 && over0.visible)
        over0.applyScreenGeometry()

    for (var i = 0; i < _overlays.length; ++i) {
        if (_overlays[i] && _overlays[i].visible)
            _overlays[i].applyScreenGeometry()
    }
}
