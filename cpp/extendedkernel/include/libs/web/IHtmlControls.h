#ifndef IHTMLCONTROLS_H
#define IHTMLCONTROLS_H

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {

/**
 * Interface for classes that apply modifications to the HTML web view of
 * SIMILAR.
 */
class IHtmlControls {
public:
  virtual ~IHtmlControls() = default;

  /**
   * Sets whether the start button is enabled or not in the view.
   * @param active true if the start button should be interactive, false if
   * deactivated
   */
  virtual void setStartButtonState(bool active) = 0;

  /**
   * Sets whether the pause button is enabled or not in the view.
   * @param active true if the pause button should be interactive, false if
   * deactivated
   */
  virtual void setPauseButtonState(bool active) = 0;

  /**
   * Sets whether the abort button is enabled or not in the view.
   * @param active true if the abort button should be interactive, false if
   * deactivated
   */
  virtual void setAbortButtonState(bool active) = 0;

  /**
   * Requests the view to shut down.
   */
  virtual void shutDownView() = 0;
};

} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IHTMLCONTROLS_H
