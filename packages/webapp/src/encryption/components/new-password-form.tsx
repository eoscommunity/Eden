import { useState } from "react";
import { PrivateKey } from "eosjs/dist/eosjs-key-conversions";
import { IoMdCopy } from "react-icons/io";

import { generateEncryptionKey, onError, useFormFields } from "_app";
import { Button, Form, Heading, Text } from "_app/ui";

interface Props {
    isLoading?: boolean;
    forgotPassword?: boolean;
    onSubmit: (publicKey: string, privateKey: string) => Promise<void>;
    onCancel: () => void;
}

export const NewPasswordForm = ({
    forgotPassword,
    isLoading,
    onSubmit,
    onCancel,
}: Props) => {
    const [didCopyText, setDidCopyText] = useState<boolean>(false);
    const [fields, setFields] = useFormFields({
        password: generateEncryptionKey().privateKey.toLegacyString(),
        passwordConfirmation: "",
    });
    const onChangeFields = (e: React.ChangeEvent<HTMLInputElement>) =>
        setFields(e);

    const doSubmit = async (e: React.FormEvent) => {
        e.preventDefault();

        if (fields.password !== fields.passwordConfirmation) {
            onError(new Error("Password confirmation does not match."));
            return;
        }

        try {
            const publicKey = PrivateKey.fromString(
                fields.password
            ).getPublicKey();
            await onSubmit(publicKey.toLegacyString(), fields.password);
        } catch (error) {
            console.error(error);
            onError(error);
        }
    };

    const copyPassword = () => {
        navigator.clipboard.writeText(fields.password);
        setDidCopyText(true);
    };

    return (
        <div className="space-y-4">
            <Heading className="mb-2.5">
                {forgotPassword
                    ? "Get new election password"
                    : "Election password"}
            </Heading>
            <Text>
                {forgotPassword
                    ? "If you have forgotten or lost your election password, you can replace it with a new one."
                    : "It looks like you don’t have an election participation password yet."}{" "}
                Please COPY AND SAVE your password somewhere safe, and confirm
                it below.
            </Text>
            <form onSubmit={doSubmit} className="space-y-3">
                <Form.LabeledSet
                    label="Your Election Password"
                    htmlFor="password"
                    className="col-span-6 sm:col-span-3"
                >
                    <div className="flex space-x-2">
                        <Form.Input
                            id="password"
                            type="text"
                            disabled
                            required
                            value={fields.password}
                            onChange={onChangeFields}
                        />
                        <Button onClick={copyPassword}>
                            {didCopyText ? (
                                "Copied!"
                            ) : (
                                <IoMdCopy
                                    color="white"
                                    size={22}
                                    className="-mx-2"
                                />
                            )}
                        </Button>
                    </div>
                </Form.LabeledSet>
                <Form.LabeledSet
                    label="Confirm Your Election Password"
                    htmlFor="passwordConfirmation"
                    className="col-span-6 sm:col-span-3"
                >
                    <Form.Input
                        id="passwordConfirmation"
                        type="text"
                        required
                        value={fields.passwordConfirmation}
                        onChange={onChangeFields}
                    />
                </Form.LabeledSet>
                <div className="flex space-x-3">
                    <Button
                        type="neutral"
                        onClick={onCancel}
                        disabled={isLoading}
                    >
                        Cancel
                    </Button>
                    <Button
                        isSubmit
                        isLoading={isLoading}
                        disabled={!fields.passwordConfirmation || isLoading}
                    >
                        {isLoading ? "Submitting..." : "Submit"}
                    </Button>
                </div>
            </form>
        </div>
    );
};
