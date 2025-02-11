/// Represent an edge in the screen.
/// This is mainly used for setting the anchor for layers.
enum ScreenEdge {
  top(1 << 0),
  right(1 << 1),
  bottom(1 << 2),
  left(1 << 3);

  /// This is an integer representation of the enum. This value must match with the
  /// int representation of the enum in the platform side.
  final int value;

  const ScreenEdge(this.value);
}
