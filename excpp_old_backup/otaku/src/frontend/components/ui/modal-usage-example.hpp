#include "..contexts/ModalContext.hpp"
#include "button.hpp"
#include "input.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * EXAMPLE USAGE OF THE MODAL SYSTEM
 * 
 * This file shows how to use the new modal system (useModal hook)
 * instead of using createPortal directly.
 * 
 * Benefits:
 * - No need to manage isOpen/onClose state manually
 * - No need to use createPortal
 * - Automatic z-index management (z-9998)
 * - Centralized modal management
 * - Similar pattern to LoadingPanel
 */

// ============================================
// EXAMPLE 1: Simple Content Modal
// ============================================
void ExampleSimpleModal();

// ============================================
// EXAMPLE 2: Form Modal (like SendModal)
// ============================================
void ExampleFormModal();

// ============================================
// EXAMPLE 3: Confirmation Modal
// ============================================
void ExampleConfirmationModal();

// ============================================
// EXAMPLE 4: Custom Styling Modal
// ============================================
void ExampleCustomStyledModal();

// ============================================
// EXAMPLE 5: Converting existing SendModal
// ============================================

/**
 * HOW TO CONVERT YOUR EXISTING MODALS:
 * 
 * OLD WAY (with createPortal):
 * ```tsx
 * void SendModal(auto { isOpen, auto onClose, auto tokens, SendModalProps userId });
 * 
 * // Usage:
 * const [isModalOpen, setIsModalOpen] = useState(false);
 * <SendModal isOpen={isModalOpen} onClose={() => setIsModalOpen(false)} />
 * ```
 * 
 * NEW WAY (with useModal):
 * ```tsx
 * // 1. Create a component for the modal content (no isOpen/onClose props needed)
 * function SendModalContent({ tokens, userId, onSuccess }: { tokens: Token[], userId: string, onSuccess: () => void }) {
 *   const { hideModal } = useModal();
 *   const modalId = 'send-modal';
 *   
 *   const handleClose = () => {
 *     hideModal(modalId);
 *   };
 *   
 *   return (
 *     <div className="space-y-4">
 *       // Your modal content here (without the portal/backdrop wrapper)
 *       // Just the inner content
 *     </div>
 *   );
 * }
 * 
 * // 2. In your component where you want to open the modal:
 * void MyComponent();
 * ```
 * 
 * BENEFITS:
 * - No need to manage isOpen state
 * - No need to import createPortal
 * - Automatic z-index management
 * - Cleaner component structure
 * - Consistent with LoadingPanel pattern
 */

// Full demo component showing all examples
void ModalExamplesDemo();


} // namespace elizaos
