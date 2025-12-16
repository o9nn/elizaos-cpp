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

;
;
;
;

// ============================================
// EXAMPLE 1: Simple Content Modal
// ============================================
 = useModal();
  const modalId = 'simple-modal';

  const handleOpenModal = () => {
    showModal(
      <div className="space-y-4">
        <h3 className="text-lg font-semibold">Simple Modal Example</h3>
        <p className="text-muted-foreground">
          This is a simple modal without using createPortal!
        </p>
        <Button onClick={() => hideModal(modalId)}>
          Close
        </Button>
      </div>,
      modalId,
      {
        closeOnBackdropClick: true,
        closeOnEsc: true,
        showCloseButton: true,
      }
    );
  };

  return (
    <Button onClick={handleOpenModal}>
      Open Simple Modal
    </Button>
  );
}

// ============================================
// EXAMPLE 2: Form Modal (like SendModal)
// ============================================
 = useModal();
  const modalId = 'form-modal';

  const handleOpenFormModal = () => {
    // You can define the form content inline or as a separate component
    const FormContent = () => {
      const [name, setName] = useState('');
      const [email, setEmail] = useState('');

      const handleSubmit = () => {
        console.log('Form submitted:', { name, email });
        // Do something with the data
        hideModal(modalId);
      };

      return (
        <div className="space-y-4">
          <div className="flex items-center justify-between">
            <h3 className="text-lg font-semibold">Form Example</h3>
          </div>

          <div className="space-y-2">
            <label className="text-sm font-medium">Name</label>
            <Input
              type="text"
              placeholder="Enter your name"
              value={name}
              onChange={(e) => setName(e.target.value)}
            />
          </div>

          <div className="space-y-2">
            <label className="text-sm font-medium">Email</label>
            <Input
              type="email"
              placeholder="Enter your email"
              value={email}
              onChange={(e) => setEmail(e.target.value)}
            />
          </div>

          <div className="flex gap-2">
            <Button
              onClick={() => hideModal(modalId)}
              variant="outline"
              className="flex-1"
            >
              Cancel
            </Button>
            <Button
              onClick={handleSubmit}
              className="flex-1"
              disabled={!name || !email}
            >
              Submit
            </Button>
          </div>
        </div>
      );
    };

    showModal(
      <FormContent />,
      modalId,
      {
        closeOnBackdropClick: false, // Don't close on backdrop click for forms
        closeOnEsc: true,
        showCloseButton: true,
      }
    );
  };

  return (
    <Button onClick={handleOpenFormModal}>
      Open Form Modal
    </Button>
  );
}

// ============================================
// EXAMPLE 3: Confirmation Modal
// ============================================
 = useModal();
  const modalId = 'confirm-modal';

  const handleDeleteAction = () => {
    showModal(
      <div className="space-y-4">
        <h3 className="text-lg font-semibold text-destructive">
          Confirm Delete
        </h3>
        <p className="text-muted-foreground">
          Are you sure you want to delete this item? This action cannot be undone.
        </p>
        <div className="flex gap-2">
          <Button
            onClick={() => hideModal(modalId)}
            variant="outline"
            className="flex-1"
          >
            Cancel
          </Button>
          <Button
            onClick={() => {
              console.log('Item deleted!');
              hideModal(modalId);
            }}
            variant="destructive"
            className="flex-1"
          >
            Delete
          </Button>
        </div>
      </div>,
      modalId,
      {
        closeOnBackdropClick: true,
        closeOnEsc: true,
        showCloseButton: false, // Hide close button for confirmation modals
      }
    );
  };

  return (
    <Button onClick={handleDeleteAction} variant="destructive">
      Delete Item
    </Button>
  );
}

// ============================================
// EXAMPLE 4: Custom Styling Modal
// ============================================
 = useModal();
  const modalId = 'custom-modal';

  const handleOpenCustomModal = () => {
    showModal(
      <div className="space-y-4">
        <h3 className="text-lg font-semibold">Custom Styled Modal</h3>
        <p className="text-muted-foreground">
          This modal has custom width and styling!
        </p>
        <Button onClick={() => hideModal(modalId)}>
          Close
        </Button>
      </div>,
      modalId,
      {
        closeOnBackdropClick: true,
        closeOnEsc: true,
        showCloseButton: true,
        className: 'max-w-2xl', // Custom width
      }
    );
  };

  return (
    <Button onClick={handleOpenCustomModal}>
      Open Custom Styled Modal
    </Button>
  );
}

// ============================================
// EXAMPLE 5: Converting existing SendModal
// ============================================

/**
 * HOW TO CONVERT YOUR EXISTING MODALS:
 * 
 * OLD WAY (with createPortal):
 * ```tsx
 * : SendModalProps) {
 *   if (!isOpen) return null;
 *   
 *   return createPortal(
 *     <div className="fixed inset-0 z-[100]...">
 *       // Modal content here
 *     </div>,
 *     document.body
 *   );
 * }
 * 
 * // Usage:
 * const [isModalOpen, setIsModalOpen] = useState(false);
 * <SendModal isOpen={isModalOpen} onClose={() => setIsModalOpen(false)} />
 * ```
 * 
 * NEW WAY (with useModal):
 * ```tsx
 * // 1. Create a component for the modal content (no isOpen/onClose props needed)
 * : { tokens: Token[], userId: string, onSuccess: () => void }) {
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
 *  = useModal();
 *   
 *   const handleOpenSendModal = () => {
 *     showModal(
 *       <SendModalContent 
 *         tokens={tokens} 
 *         userId={userId}
 *         onSuccess={handleSuccess}
 *       />,
 *       'send-modal',
 *       { closeOnBackdropClick: true }
 *     );
 *   };
 *   
 *   return <Button onClick={handleOpenSendModal}>Send</Button>;
 * }
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



} // namespace elizaos
