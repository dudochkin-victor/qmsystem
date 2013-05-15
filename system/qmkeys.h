/*!
 * @file qmkeys.h
 * @brief Contains QmKeys which provides access to hardware key states.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

   @scope Nokia Meego

   This file is part of SystemSW QtAPI.

   SystemSW QtAPI is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   SystemSW QtAPI is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with SystemSW QtAPI.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#ifndef __QMKEYS_H__
#define __QMKEYS_H__

#include "system_global.h"
#include <QtCore/qobject.h>
QT_BEGIN_HEADER

namespace MeeGo
{

class QmKeysPrivate;

/*!
 * @scope Nokia Meego
 *
 * @class QmKeys
 * @brief QmKeys Provides access to hardware key states.
 *
 * @details Note that QmKeys works regardless of the tklock state. It is the applications
 * responsibility to, for example, use QmLocks to see if the keys are locked. QmKeys supports one bluetooth headset.
 * Still very preliminary, missing most of its functionality.
 *
 * @todo <ul>
 *       <li>Move keyboard slider to MCE signal..?</li>
 *       <li>Add query functions to figure out whether handles are valid or not.</li>
 *       </ul>
 */
class MEEGO_SYSTEM_EXPORT QmKeys : public QObject
{
  Q_OBJECT

  Q_ENUMS(KeyboardSliderPosition)
  Q_ENUMS(LensCoverPosition)
  Q_ENUMS(CameraKeyPosition)
  Q_ENUMS(Key)
  Q_ENUMS(State)

public:

  //! Position of keyboard slider
  enum KeyboardSliderPosition
  {
    KeyboardSliderIn = 0,   //!< Slider is in
    KeyboardSliderOut       //!< Slider is out
  };

  //! Position of lens cover
  enum LensCoverPosition
  {
    LensCovered = 0,        //!< Lens is covered
    LensUnCovered,           //!< Lens is not covered
    Unknown
  };

  //! Position of the camera launch key
  enum CameraKeyPosition
  {
    Up = 0, //!< Camera key is up
    Down,   //!< Camera key is half down
    Through, //!< Camera key is fully down
    Invalid
  };

  //! Supported keys
  enum Key
  {
      UnknownKey = -1,
      KeyboardSlider = 0,
      Camera,
      VolumeUp,
      VolumeDown,
      Phone,
      PlayPause,
      Stop,
      Forward,
      Rewind,
      Mute,
      LeftKey,
      RightKey,
      UpKey,
      DownKey,
      End,
      NextSong,
      PreviousSong,
      Pause,
      Play,
      RightCtrl,
      PowerKey
  };

  //! Supported positions for keys
  enum State
  {
      KeyUp = 0, //!< The key is up
      KeyHalfDown, //!< The key is half down
      KeyDown, //!< The key is fully down
      KeyInvalid //!< The key state is invalid or unknown
  };

public:
  /*!
   * @brief Constructor
   * @param parent The possible parent object
   */
  QmKeys(QObject *parent = 0);


  /*!
   * @brief Destructor
   *
   * Destruct QmKeys when you do not expect key pressures (for
   * example when screen is blanked) to save battery.
   *
   */
  virtual ~QmKeys();

  /*!
   * @brief Deprecated, use getKeyState() instead
   * @deprecated Deprecated, use getKeyState() instead
   * @return Current keyboard slider position
   */
  QmKeys::KeyboardSliderPosition getSliderPosition();

  /*!
   * @brief Deprecated, use getKeyState() instead
   * @deprecated Deprecated, use getKeyState() instead
   * @return Current lens cover position
   */
  QmKeys::LensCoverPosition getLensCoverPosition();

  /*!
   * @brief Deprecated, use getKeyState() instead
   * @deprecated Deprecated, use getKeyState() instead
   * @return Current state of the camera launcher key
   */
  QmKeys::CameraKeyPosition getCameraLauncherPosition();

  /*!
   * @brief Deprecated, use getKeyState() instead
   * @deprecated Deprecated, use getKeyState() instead
   * @return Is the volume up key down
   */
  bool isVolumeUpDown();

  /*!
   * @brief Deprecated, use getKeyState() instead
   * @deprecated Deprecated, use getKeyState() instead
   * @return Is the volume down key down
   */
  bool isVolumeDownDown();

  /*!
   * @brief Gets the current state of the given key.
   * @param key which state is being queried
   * @return The state of the given key
   */
  State getKeyState(Key);

Q_SIGNALS:

  /*!
   * @brief Deprecated, use keyEvent() instead.
   * @deprecated Deprecated, use keyEvent() instead.
   * @param where New slider position
   */
  void keyboardSliderMoved(QmKeys::KeyboardSliderPosition where);

  /*!
   * @brief Deprecated, use keyEvent() instead.
   * @deprecated Deprecated, use keyEvent() instead.
   * @param where New cover position
   */
  void lensCoverMoved(QmKeys::LensCoverPosition where);

  /*!
   * @brief Deprecated, use keyEvent() instead.
   * @deprecated Deprecated, use keyEvent() instead.
   * @param where New camera luncher position
   */
  void cameraLauncherMoved(QmKeys::CameraKeyPosition);

  /*!
   * @brief Deprecated, use keyEvent() instead.
   * @deprecated Deprecated, use keyEvent() instead.
   * @param Is the volume up key down
   */
  void volumeUpMoved(bool);

  /*!
   * @brief Deprecated, use keyEvent() instead.
   * @deprecated Deprecated, use keyEvent() instead.
   * @param Is the volume down key down
   */
  void volumeDownMoved(bool);

  /*!
   * @brief Sent when a key has changed state.
   * @param key the key in question
   * @param state The new state
   */
  void keyEvent(MeeGo::QmKeys::Key key, MeeGo::QmKeys::State state);

private:
        Q_DISABLE_COPY(QmKeys)
        QmKeysPrivate *priv;
};

} // MeeGo namespace

QT_END_HEADER

#endif // __QMKEYS_H__

