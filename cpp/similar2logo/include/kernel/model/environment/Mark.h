#ifndef SIMILAR2LOGO_MARK_H
#define SIMILAR2LOGO_MARK_H

#include "../../tools/Point2D.h"
#include "SituatedEntity.h"
#include <memory>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace model {
namespace environment {

/**
 * Represents a mark dropped by an agent in the environment.
 * Marks can have content (any type) and a category for classification.
 *
 * Template parameter T is the type of content stored in the mark.
 */
template <typename T> class Mark : public SituatedEntity {
private:
  ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::Point2D
      location;
  T content;
  std::string category;

public:
  /**
   * Creates a mark with location, content, and category.
   * @param location The location of the mark
   * @param content The content of the mark
   * @param category The category of the mark
   */
  Mark(const ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::
           Point2D &location,
       const T &content, const std::string &category)
      : location(location), content(content), category(category) {}

  /**
   * Creates a mark with location and content (no category).
   * @param location The location of the mark
   * @param content The content of the mark
   */
  Mark(const ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::
           Point2D &location,
       const T &content)
      : location(location), content(content), category("") {}

  /**
   * Creates a mark with only location (default content and no category).
   * @param location The location of the mark
   */
  explicit Mark(const ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::
                    tools::Point2D &location)
      : location(location), content(T()), category("") {}

  /**
   * Creates a mark with location and category (default content).
   * @param location The location of the mark
   * @param category The category of the mark
   */
  Mark(const ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::
           Point2D &location,
       const std::string &category)
      : location(location), content(T()), category(category) {}

  virtual ~Mark() = default;

  // SituatedEntity interface
  ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::Point2D
  getLocation() const override {
    return location;
  }

  /**
   * Gets the content of the mark.
   * @return The content of the mark
   */
  const T &getContent() const { return content; }

  /**
   * Gets mutable reference to the content.
   * @return Reference to the content
   */
  T &getContent() { return content; }

  /**
   * Sets the content of the mark.
   * @param newContent The new content
   */
  void setContent(const T &newContent) { content = newContent; }

  /**
   * Gets the category of the mark.
   * @return The category of the mark
   */
  const std::string &getCategory() const { return category; }

  /**
   * Creates a copy of this mark.
   * @return A new mark with the same properties
   */
  std::shared_ptr<Mark<T>> clone() const {
    return std::make_shared<Mark<T>>(location, content, category);
  }
};

// Common mark types - using double as default instead of std::any
using SimpleMark = Mark<double>;

// Common mark types
using IntMark = Mark<int>;
using DoubleMark = Mark<double>;
using StringMark = Mark<std::string>;

} // namespace environment
} // namespace model
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_MARK_H
