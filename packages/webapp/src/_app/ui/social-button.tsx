import React from "react";
import { IconType } from "react-icons/lib";

interface Props {
    handle: string;
    href: string;
    icon: IconType;
    color?: string;
    size?: number;
    className?: string;
}

const BASE_CLASS = "hover:underline space-x-3 p-1 flex-grow flex flex-wrap";

export const SocialButton = ({
    handle,
    color,
    size = 6,
    icon,
    href,
    className,
}: Props) => {
    const buttonColor = `text-${color || "gray"}-500`;
    const buttonClass = `${className || ""} ${buttonColor} ` + BASE_CLASS;

    const formattedIcon = React.createElement(icon, {
        className: `w-${size} h-${size} inline-flex`,
    });

    return (
        <a href={href} className={buttonClass} target="_blank">
            {formattedIcon}
            <span>{handle}</span>
        </a>
    );
};

export default SocialButton;
