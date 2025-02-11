enum WindowLayer {
  background(1),
  bottom(2),
  top(3),
  overlay(4);

  /// This is the ID of the layer. This should match with the int representation
  /// of layers in the platform side enum.
  final int layerId;

  const WindowLayer(this.layerId);
}
