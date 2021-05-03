import {
    FaAirbnb,
    FaBitcoin,
    FaBlogger,
    FaTelegram,
    FaTwitter,
} from "react-icons/fa";

import { SocialButton } from "_app";
import { MemberData } from "../interfaces";

interface Props {
    member: MemberData;
}

export const MemberSocialLinks = ({ member }: Props) => (
    <nav className="space-y-2 flex flex-col">
        <SocialButton
            handle={member.account}
            icon={FaBitcoin}
            color="black"
            href={`https://bloks.io/account/${member.account}`}
        />
        {member.socialHandles.telegram && (
            <SocialButton
                handle={member.socialHandles.telegram}
                icon={FaTelegram}
                color="indigo"
                href={`https://t.me/${member.socialHandles.telegram}`}
            />
        )}
        {member.socialHandles.twitter && (
            <SocialButton
                handle={member.socialHandles.twitter}
                icon={FaTwitter}
                color="blue"
                href={`https://twitter.com/${member.socialHandles.twitter}`}
            />
        )}
        {member.socialHandles.eosCommunity && (
            <SocialButton
                handle={member.socialHandles.eosCommunity}
                icon={FaAirbnb}
                color="red"
                href={`https://eoscommunity.org/u/${member.socialHandles.eosCommunity}`}
            />
        )}
        {member.socialHandles.blog && (
            <SocialButton
                handle="Blog"
                icon={FaBlogger}
                color="yellow"
                href={member.socialHandles.blog}
            />
        )}
    </nav>
);
